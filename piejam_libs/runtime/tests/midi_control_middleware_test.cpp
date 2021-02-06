// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "middleware_functors_mock.h"

#include <piejam/midi/device_update.h>
#include <piejam/runtime/actions/refresh_midi_devices.h>
#include <piejam/runtime/midi_control_middleware.h>
#include <piejam/runtime/ui/action.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace piejam::runtime::test
{

struct midi_control_middleware_test : testing::Test
{
    testing::StrictMock<middleware_functors_mock> mf_mock;
};

TEST_F(midi_control_middleware_test, unknown_action_is_passed_to_next)
{
    struct some_action final : public action
    {
        auto clone() const -> std::unique_ptr<action>
        {
            return std::make_unique<some_action>();
        }

        auto reduce(state const& st) const -> state { return st; }
    };

    midi_control_middleware sut(make_middleware_functors(mf_mock), {});

    some_action action;
    EXPECT_CALL(mf_mock, next(testing::Ref(action)));
    sut(action);
}

TEST_F(midi_control_middleware_test,
       for_refresh_midi_devices_action_and_no_updates_no_action_is_propagated)
{
    midi_control_middleware sut(make_middleware_functors(mf_mock), {});

    actions::refresh_midi_devices action;
    sut(action);
}

TEST_F(midi_control_middleware_test, refresh_midi_devices_and_add_update)
{
    using testing::_;

    auto dev_id = midi::device_id_t::generate();
    midi_control_middleware sut(
            make_middleware_functors(mf_mock),
            [dev_id]() -> std::vector<midi::device_update> {
                return {midi::device_added{
                        .device_id = dev_id,
                        .name = "test"}};
            });

    state st;

    actions::refresh_midi_devices action;
    EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
        st = a.reduce(st);
    });
    sut(action);

    ASSERT_EQ(1u, st.midi_inputs->size());
    EXPECT_EQ(dev_id, st.midi_inputs->at(0));

    ASSERT_TRUE(st.midi_devices->contains(dev_id));
    EXPECT_EQ("test", st.midi_devices->at(dev_id).name);
    EXPECT_EQ(false, st.midi_devices->at(dev_id).enabled);
}

TEST_F(midi_control_middleware_test,
       refresh_midi_devices_and_add_and_remove_update)
{
    using testing::_;

    auto dev_id = midi::device_id_t::generate();
    midi_control_middleware sut(
            make_middleware_functors(mf_mock),
            [dev_id]() -> std::vector<midi::device_update> {
                return {midi::device_added{.device_id = dev_id, .name = "test"},
                        midi::device_removed{.device_id = dev_id}};
            });

    state st;

    actions::refresh_midi_devices action;
    EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
        st = a.reduce(st);
    });
    sut(action);

    EXPECT_TRUE(st.midi_inputs->empty());
    EXPECT_TRUE(st.midi_devices->empty());
}

TEST_F(midi_control_middleware_test,
       refresh_midi_devices_and_add_and_remove_nonexistent_update)
{
    using testing::_;

    auto dev_id = midi::device_id_t::generate();
    midi_control_middleware sut(
            make_middleware_functors(mf_mock),
            [dev_id]() -> std::vector<midi::device_update> {
                return {midi::device_added{.device_id = dev_id, .name = "test"},
                        midi::device_removed{
                                .device_id = midi::device_id_t::generate()}};
            });

    state st;

    actions::refresh_midi_devices action;
    EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
        st = a.reduce(st);
    });
    sut(action);

    ASSERT_EQ(1u, st.midi_inputs->size());
    EXPECT_EQ(dev_id, st.midi_inputs->at(0));

    ASSERT_TRUE(st.midi_devices->contains(dev_id));
    EXPECT_EQ("test", st.midi_devices->at(dev_id).name);
    EXPECT_EQ(false, st.midi_devices->at(dev_id).enabled);
}

} // namespace piejam::runtime::test
