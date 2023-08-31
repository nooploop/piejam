// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "audio_device_manager_mock.h"
#include "ladspa_processor_factory_mock.h"
#include "middleware_functors_mock.h"

#include <piejam/audio/device.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/period_count.h>
#include <piejam/audio/period_size.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/runtime/actions/initiate_device_selection.h>
#include <piejam/runtime/actions/select_period_size.h>
#include <piejam/runtime/actions/select_sample_rate.h>
#include <piejam/runtime/audio_engine_middleware.h>
#include <piejam/runtime/midi_input_controller.h>
#include <piejam/runtime/state.h>
#include <piejam/thread/configuration.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace piejam::runtime::test
{

struct audio_engine_middleware_test : ::testing::Test
{
    testing::StrictMock<middleware_functors_mock> mf_mock;
    testing::StrictMock<audio_device_manager_mock> audio_device_manager;
    testing::StrictMock<ladspa_processor_factory_mock> ladspa_processor_factory;

    audio_engine_middleware sut{
            {},
            {},
            audio_device_manager,
            ladspa_processor_factory,
            nullptr};
};

TEST_F(audio_engine_middleware_test,
       non_device_actions_are_passed_directly_to_next)
{
    struct non_device_action : reducible_action
    {
        [[nodiscard]] auto clone() const -> std::unique_ptr<action> override
        {
            return std::make_unique<non_device_action>();
        }

        void reduce(state&) const override
        {
        }
    } action;
    EXPECT_CALL(mf_mock, next(::testing::Ref(action)));
    sut(make_middleware_functors(mf_mock), action);
}

TEST_F(audio_engine_middleware_test,
       select_sample_rate_is_ignored_if_index_is_not_in_current_sample_rates)
{
    using namespace testing;

    state st;
    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));

    actions::select_sample_rate action;
    sut(make_middleware_functors(mf_mock), action);
}

TEST_F(audio_engine_middleware_test, select_sample_rate_will_change_sample_rate)
{
    using namespace testing;

    audio::pcm_hw_params const default_hw_params{
            .sample_rates =
                    {audio::sample_rate(44100u), audio::sample_rate(48000u)},
            .period_sizes = {},
            .period_counts = {}};

    state st;
    st.pcm_devices = audio::pcm_io_descriptors{
            .inputs = {audio::pcm_descriptor{.name = "foo", .path = {}}},
            .outputs = {audio::pcm_descriptor{.name = "foo", .path = {}}}};
    st.input.index = 0;
    st.input.hw_params = default_hw_params;
    st.output.index = 0;
    st.output.hw_params = default_hw_params;
    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));
    EXPECT_CALL(mf_mock, next(_)).WillRepeatedly([&st](auto const& a) {
        dynamic_cast<reducible_action const&>(a).reduce(st);
    });
    EXPECT_CALL(audio_device_manager, hw_params(_, _, _))
            .WillRepeatedly(Return(default_hw_params));

    actions::select_sample_rate action;
    action.index = 1;

    ASSERT_EQ(audio::sample_rate(), st.sample_rate);
    sut(make_middleware_functors(mf_mock), action);

    EXPECT_EQ(audio::sample_rate(48000u), st.sample_rate);
}

TEST_F(audio_engine_middleware_test, select_period_size_will_change_period_size)
{
    using namespace testing;

    audio::pcm_hw_params const default_hw_params{
            .sample_rates =
                    {audio::sample_rate(44100u), audio::sample_rate(48000u)},
            .period_sizes = {audio::period_size(64u), audio::period_size(128u)},
            .period_counts = {}};

    state st;
    st.pcm_devices = audio::pcm_io_descriptors{
            .inputs = {audio::pcm_descriptor{.name = "foo", .path = {}}},
            .outputs = {audio::pcm_descriptor{.name = "foo", .path = {}}}};
    st.input.index = 0;
    st.input.hw_params = default_hw_params;
    st.output.index = 0;
    st.output.hw_params = default_hw_params;
    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));
    EXPECT_CALL(mf_mock, next(_)).WillRepeatedly([&st](auto const& a) {
        dynamic_cast<reducible_action const&>(a).reduce(st);
    });
    EXPECT_CALL(audio_device_manager, hw_params(_, _, _))
            .WillRepeatedly(Return(default_hw_params));

    actions::select_period_size action;
    action.index = 1;

    ASSERT_EQ(audio::period_size(), st.period_size);
    sut(make_middleware_functors(mf_mock), action);

    EXPECT_EQ(audio::period_size(128u), st.period_size);
}

TEST_F(audio_engine_middleware_test,
       initiate_device_selection_is_converted_to_select_device_and_passed_to_next)
{
    using namespace testing;

    piejam::audio::pcm_hw_params hw_params{
            .sample_rates = {audio::sample_rate(44100u)},
            .period_sizes = {audio::period_size(128u)},
            .period_counts = {audio::period_count(2u)}};

    state st;
    st.input.index = 0;
    st.input.hw_params = hw_params;
    st.output.index = 0;
    st.output.hw_params = hw_params;
    st.pcm_devices = audio::pcm_io_descriptors{
            .inputs =
                    {audio::pcm_descriptor{.name = "foo", .path = {}},
                     audio::pcm_descriptor{.name = "bar", .path = {}}},
            .outputs = {audio::pcm_descriptor{.name = "foo", .path = {}}}};

    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));
    EXPECT_CALL(mf_mock, next(_)).WillRepeatedly([&st](auto const& a) {
        dynamic_cast<reducible_action const&>(a).reduce(st);
    });
    EXPECT_CALL(audio_device_manager, hw_params(_, _, _))
            .WillRepeatedly(Return(hw_params));
    EXPECT_CALL(audio_device_manager, make_device(_, _, _))
            .WillRepeatedly(Return(ByMove(audio::make_dummy_device())));

    actions::initiate_device_selection in_action;
    in_action.input = true;
    in_action.index = 1; // select another device

    sut(make_middleware_functors(mf_mock), in_action);

    EXPECT_EQ(1u, st.input.index);
}

} // namespace piejam::runtime::test
