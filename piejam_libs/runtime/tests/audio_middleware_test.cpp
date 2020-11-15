// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <piejam/runtime/actions/device_actions.h>
#include <piejam/runtime/audio_engine_middleware.h>
#include <piejam/runtime/audio_state.h>
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
                std::unique_ptr<audio::device>(audio_state const&));
        MOCK_CONST_METHOD0(get_state, audio_state const&());
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
                [this](audio_state const& st)
                        -> std::unique_ptr<audio::device> {
                    return m_ctrl.create_device(st);
                },
                [this]() -> audio_state const& { return m_ctrl.get_state(); },
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

        auto reduce(audio_state const& st) const -> audio_state override
        {
            return st;
        }
    } action;
    EXPECT_CALL(m_ctrl, next(::testing::Ref(action)));
    make_sut()(action);
}

TEST_F(audio_engine_middleware_test, select_samplerate_is_passed_to_next)
{
    using namespace testing;
    EXPECT_CALL(m_ctrl, get_state())
            .WillRepeatedly(ReturnRefOfCopy(audio_state()));
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
    EXPECT_CALL(m_ctrl, get_state())
            .WillRepeatedly(ReturnRefOfCopy(audio_state()));
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

    audio_state st;
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

    actions::select_device out_action;
    out_action.input = true;
    out_action.device.index = 1;
    out_action.device.hw_params = hw_params;

    EXPECT_CALL(m_ctrl, get_hw_params(Ref(st.pcm_devices->inputs[1])))
            .WillOnce(Return(hw_params));
    EXPECT_CALL(
            m_ctrl,
            next(WhenDynamicCastTo<actions::select_device const&>(out_action)));

    make_sut()(in_action);
}

} // namespace piejam::runtime::test
