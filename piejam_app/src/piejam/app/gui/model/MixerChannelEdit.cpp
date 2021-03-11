// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/MixerChannelEdit.h>

#include <piejam/app/gui/model/MidiAssignable.h>
#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/actions/mixer_actions.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::app::gui::model
{

struct MixerChannelEdit::Impl
{
    runtime::mixer::channel_id channelId;

    boxed_vector<runtime::selectors::mixer_device_route> inputDevices;
    boxed_vector<runtime::selectors::mixer_channel_route> inputChannels;
    boxed_vector<runtime::selectors::mixer_device_route> outputDevices;
    boxed_vector<runtime::selectors::mixer_channel_route> outputChannels;
};

MixerChannelEdit::MixerChannelEdit(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(id))
{
}

MixerChannelEdit::~MixerChannelEdit() = default;

void
MixerChannelEdit::onSubscribe()
{
    observe(runtime::selectors::make_mixer_channel_name_selector(
                    m_impl->channelId),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    observe(runtime::selectors::make_mixer_channel_can_move_left_selector(
                    m_impl->channelId),
            [this](bool const x) { setCanMoveLeft(x); });

    observe(runtime::selectors::make_mixer_channel_can_move_right_selector(
                    m_impl->channelId),
            [this](bool const x) { setCanMoveRight(x); });

    observe(runtime::selectors::
                    make_default_mixer_channel_input_is_valid_selector(
                            m_impl->channelId),
            [this](bool const x) { setDefaultInputIsValid(x); });

    observe(runtime::selectors::make_mixer_channel_input_selector(
                    m_impl->channelId),
            [this](box<runtime::selectors::selected_route> const& sel_route) {
                setSelectedInput(QString::fromStdString(sel_route->name));
                switch (sel_route->state)
                {
                    case runtime::selectors::selected_route::state_t::valid:
                        setSelectedInputState(SelectedInputState::Valid);
                        break;

                    default:
                        setSelectedInputState(SelectedInputState::Invalid);
                        break;
                }
            });

    observe(runtime::selectors::select_mixer_input_devices,
            [this](boxed_vector<runtime::selectors::mixer_device_route> const&
                           inputDevices) {
                m_impl->inputDevices = inputDevices;

                QStringList inputDeviceNames;
                std::ranges::transform(
                        *inputDevices,
                        std::back_inserter(inputDeviceNames),
                        &QString::fromStdString,
                        &runtime::selectors::mixer_device_route::name);
                setInputDevices(std::move(inputDeviceNames));
            });

    observe(runtime::selectors::make_mixer_input_channels_selector(
                    m_impl->channelId),
            [this](boxed_vector<runtime::selectors::mixer_channel_route> const&
                           inputChannels) {
                m_impl->inputChannels = inputChannels;

                QStringList inputChannelNames;
                std::ranges::transform(
                        *inputChannels,
                        std::back_inserter(inputChannelNames),
                        &QString::fromStdString,
                        &runtime::selectors::mixer_channel_route::name);
                setInputChannels(std::move(inputChannelNames));
            });

    observe(runtime::selectors::make_mixer_channel_output_selector(
                    m_impl->channelId),
            [this](box<runtime::selectors::selected_route> const& sel_route) {
                setSelectedOutput(QString::fromStdString(sel_route->name));
                switch (sel_route->state)
                {
                    case runtime::selectors::selected_route::state_t::invalid:
                        setSelectedOutputState(SelectedOutputState::Invalid);
                        break;

                    case runtime::selectors::selected_route::state_t::valid:
                        setSelectedOutputState(SelectedOutputState::Valid);
                        break;

                    case runtime::selectors::selected_route::state_t::not_mixed:
                        setSelectedOutputState(SelectedOutputState::NotMixed);
                        break;
                }
            });

    observe(runtime::selectors::select_mixer_output_devices,
            [this](boxed_vector<runtime::selectors::mixer_device_route> const&
                           outputDevices) {
                m_impl->outputDevices = outputDevices;

                QStringList outputDeviceNames;
                std::ranges::transform(
                        *outputDevices,
                        std::back_inserter(outputDeviceNames),
                        &QString::fromStdString,
                        &runtime::selectors::mixer_device_route::name);
                setOutputDevices(std::move(outputDeviceNames));
            });

    observe(runtime::selectors::make_mixer_output_channels_selector(
                    m_impl->channelId),
            [this](boxed_vector<runtime::selectors::mixer_channel_route> const&
                           outputChannels) {
                m_impl->outputChannels = outputChannels;

                QStringList outputChannelNames;
                std::ranges::transform(
                        *outputChannels,
                        std::back_inserter(outputChannelNames),
                        &QString::fromStdString,
                        &runtime::selectors::mixer_channel_route::name);
                setOutputChannels(std::move(outputChannelNames));
            });
}

void
MixerChannelEdit::changeName(QString const& name)
{
    runtime::actions::set_mixer_channel_name action;
    action.channel_id = m_impl->channelId;
    action.name = name.toStdString();
    dispatch(action);
}

void
MixerChannelEdit::moveLeft()
{
    BOOST_ASSERT(canMoveLeft());
    runtime::actions::move_mixer_channel_left action;
    action.channel_id = m_impl->channelId;
    dispatch(action);
}

void
MixerChannelEdit::moveRight()
{
    BOOST_ASSERT(canMoveRight());
    runtime::actions::move_mixer_channel_right action;
    action.channel_id = m_impl->channelId;
    dispatch(action);
}

void
MixerChannelEdit::deleteChannel()
{
    dispatch(runtime::actions::initiate_mixer_channel_deletion(
            m_impl->channelId));
}

void
MixerChannelEdit::changeInputToMix()
{
    runtime::actions::set_mixer_channel_input action;
    action.channel_id = m_impl->channelId;
    dispatch(action);
}

void
MixerChannelEdit::changeInputToDevice(unsigned index)
{
    BOOST_ASSERT(index < m_impl->inputDevices->size());

    runtime::actions::set_mixer_channel_input action;
    action.channel_id = m_impl->channelId;
    action.route = (*m_impl->inputDevices)[index].bus_id;
    dispatch(action);
}

void
MixerChannelEdit::changeInputToChannel(unsigned index)
{
    BOOST_ASSERT(index < m_impl->inputChannels->size());

    runtime::actions::set_mixer_channel_input action;
    action.channel_id = m_impl->channelId;
    action.route = (*m_impl->inputChannels)[index].channel_id;
    dispatch(action);
}

void
MixerChannelEdit::changeOutputToNone()
{
    runtime::actions::set_mixer_channel_output action;
    action.channel_id = m_impl->channelId;
    dispatch(action);
}

void
MixerChannelEdit::changeOutputToDevice(unsigned index)
{
    BOOST_ASSERT(index < m_impl->outputDevices->size());

    runtime::actions::set_mixer_channel_output action;
    action.channel_id = m_impl->channelId;
    action.route = (*m_impl->outputDevices)[index].bus_id;
    dispatch(action);
}

void
MixerChannelEdit::changeOutputToChannel(unsigned index)
{
    BOOST_ASSERT(index < m_impl->outputChannels->size());

    runtime::actions::set_mixer_channel_output action;
    action.channel_id = m_impl->channelId;
    action.route = (*m_impl->outputChannels)[index].channel_id;
    dispatch(action);
}

} // namespace piejam::app::gui::model
