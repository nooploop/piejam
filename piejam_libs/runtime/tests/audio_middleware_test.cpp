// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/initiate_device_selection.h>
#include <piejam/runtime/actions/select_period_size.h>
#include <piejam/runtime/actions/select_samplerate.h>
#include <piejam/runtime/audio_engine_middleware.h>
#include <piejam/runtime/state.h>
#include <piejam/thread/configuration.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace piejam::runtime::test
{

struct audio_engine_middleware_test : ::testing::Test
{
    struct controller
    {
        MOCK_CONST_METHOD0(get_pcm_io_descriptors, audio::pcm_io_descriptors());
        MOCK_CONST_METHOD1(
                get_hw_params,
                audio::pcm_hw_params(audio::pcm_descriptor const&));
        MOCK_CONST_METHOD1(
                create_device,
                std::unique_ptr<audio::device>(state const&));
        MOCK_CONST_METHOD0(get_state, state const&());
        MOCK_CONST_METHOD1(next, void(action const&));
    };

    ::testing::StrictMock<controller> m_ctrl;

    auto make_sut()
    {
        return audio_engine_middleware(
                {},
                {},
                [this]() -> audio::pcm_io_descriptors {
                    return m_ctrl.get_pcm_io_descriptors();
                },
                [this](audio::pcm_descriptor const& d) -> audio::pcm_hw_params {
                    return m_ctrl.get_hw_params(d);
                },
                [this](state const& st) -> std::unique_ptr<audio::device> {
                    return m_ctrl.create_device(st);
                },
                [this]() -> state const& { return m_ctrl.get_state(); },
                [this](action const& a) { m_ctrl.next(a); });
    }
};

TEST_F(audio_engine_middleware_test,
       non_device_actions_are_passed_directly_to_next)
{
    struct non_device_action : action
    {
        auto clone() const -> std::unique_ptr<action> override
        {
            return std::make_unique<non_device_action>();
        }

        auto reduce(state const& st) const -> state override { return st; }
    } action;
    EXPECT_CALL(m_ctrl, next(::testing::Ref(action)));
    make_sut()(action);
}

TEST_F(audio_engine_middleware_test, select_samplerate_is_passed_to_next)
{
    using namespace testing;
    EXPECT_CALL(m_ctrl, get_state()).WillRepeatedly(ReturnRefOfCopy(state()));
    EXPECT_CALL(m_ctrl, create_device(_))
            .WillOnce(Return(
                    ByMove(std::make_unique<piejam::audio::dummy_device>())));

    actions::select_samplerate action;
    EXPECT_CALL(m_ctrl, next(Ref(action)));

    make_sut()(action);
}

TEST_F(audio_engine_middleware_test, select_period_size_is_passed_to_next)
{
    using namespace testing;
    EXPECT_CALL(m_ctrl, get_state()).WillRepeatedly(ReturnRefOfCopy(state()));
    EXPECT_CALL(m_ctrl, create_device(_))
            .WillOnce(Return(
                    ByMove(std::make_unique<piejam::audio::dummy_device>())));

    actions::select_period_size action;
    EXPECT_CALL(m_ctrl, next(Ref(action)));

    make_sut()(action);
}

TEST_F(audio_engine_middleware_test,
       initiate_device_selection_is_converted_to_select_device_and_passed_to_next)
{
    using namespace testing;

    piejam::audio::pcm_hw_params hw_params;
    hw_params.samplerates = {44100};
    hw_params.period_sizes = {128};

    state st;
    st.input.hw_params = hw_params;
    st.output.hw_params = hw_params;

    audio::pcm_io_descriptors descs;
    descs.inputs.resize(2);
    st.pcm_devices = std::move(descs);

    EXPECT_CALL(m_ctrl, get_state()).WillRepeatedly(ReturnRef(st));
    EXPECT_CALL(m_ctrl, create_device(_))
            .WillOnce(Return(
                    ByMove(std::make_unique<piejam::audio::dummy_device>())));

    actions::initiate_device_selection in_action;
    in_action.input = true;
    in_action.index = 1; // select another device

    EXPECT_CALL(m_ctrl, get_hw_params(Ref(st.pcm_devices->inputs[1])))
            .WillOnce(Return(hw_params));
    EXPECT_CALL(m_ctrl, next(_));

    make_sut()(in_action);
}

} // namespace piejam::runtime::test
