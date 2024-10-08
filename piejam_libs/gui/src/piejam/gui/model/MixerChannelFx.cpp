// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerChannelFx.h>

#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/FxChainModule.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/math.h>
#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/actions/move_fx_module.h>
#include <piejam/runtime/actions/root_view_actions.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

struct MixerChannelFx::Impl
{
    box<runtime::fx::chain_t> fx_chain;

    FxChainModulesList modules;
};

MixerChannelFx::MixerChannelFx(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const mixer_channel_id)
    : MixerChannel{store_dispatch, state_change_subscriber, mixer_channel_id}
    , m_impl{make_pimpl<Impl>()}
{
}

auto
MixerChannelFx::fxChain() const noexcept -> QAbstractListModel*
{
    return &m_impl->modules;
}

void
MixerChannelFx::appendFxModule()
{
    runtime::actions::show_fx_browser action;
    action.fx_chain_id = channel_id();
    dispatch(action);
}

void
MixerChannelFx::moveUpFxModule()
{
    BOOST_ASSERT(m_canMoveUpFxModule);

    dispatch(runtime::actions::move_fx_module_up{});
}

void
MixerChannelFx::moveDownFxModule()
{
    BOOST_ASSERT(m_canMoveDownFxModule);

    dispatch(runtime::actions::move_fx_module_down{});
}

void
MixerChannelFx::onSubscribe()
{
    MixerChannel::onSubscribe();

    observe(runtime::selectors::select_focused_fx_chain,
            [this](auto const mixer_channel_id) {
                setFocused(channel_id() == mixer_channel_id);
            });

    observe(runtime::selectors::make_fx_module_can_move_up_selector(
                    channel_id()),
            [this](bool const x) { setCanMoveUpFxModule(x); });

    observe(runtime::selectors::make_fx_module_can_move_down_selector(
                    channel_id()),
            [this](bool const x) { setCanMoveDownFxModule(x); });

    observe(runtime::selectors::make_fx_chain_selector(channel_id()),
            [this](auto const& fx_chain) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(*m_impl->fx_chain, *fx_chain),
                        piejam::gui::generic_list_model_edit_script_executor{
                                m_impl->modules,
                                [this](runtime::fx::module_id const&
                                               fx_mod_id) {
                                    return std::make_unique<FxChainModule>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            channel_id(),
                                            fx_mod_id);
                                }});

                m_impl->fx_chain = fx_chain;
            });
}

} // namespace piejam::gui::model
