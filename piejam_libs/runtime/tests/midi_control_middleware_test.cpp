// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "middleware_functors_mock.h"

#include <piejam/midi/device_update.h>
#include <piejam/runtime/actions/activate_midi_device.h>
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

    EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
        st = a.reduce(st);
    });
    actions::refresh_midi_devices action;
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
    using testing::ReturnRef;

    auto dev_id = midi::device_id_t::generate();
    midi_control_middleware sut(
            make_middleware_functors(mf_mock),
            [dev_id]() -> std::vector<midi::device_update> {
                return {midi::device_added{.device_id = dev_id, .name = "test"},
                        midi::device_removed{.device_id = dev_id}};
            });

    state st;

    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));
    EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
        st = a.reduce(st);
    });
    actions::refresh_midi_devices action;
    sut(action);

    EXPECT_TRUE(st.midi_inputs->empty());
    EXPECT_TRUE(st.midi_devices->empty());
}

TEST_F(midi_control_middleware_test,
       refresh_midi_devices_and_add_and_remove_nonexistent_update)
{
    using testing::_;
    using testing::ReturnRef;

    auto dev_id = midi::device_id_t::generate();
    midi_control_middleware sut(
            make_middleware_functors(mf_mock),
            [dev_id]() -> std::vector<midi::device_update> {
                return {midi::device_added{.device_id = dev_id, .name = "test"},
                        midi::device_removed{
                                .device_id = midi::device_id_t::generate()}};
            });

    state st;

    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));
    EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
        st = a.reduce(st);
    });
    actions::refresh_midi_devices action;
    sut(action);

    ASSERT_EQ(1u, st.midi_inputs->size());
    EXPECT_EQ(dev_id, st.midi_inputs->at(0));

    ASSERT_TRUE(st.midi_devices->contains(dev_id));
    EXPECT_EQ("test", st.midi_devices->at(dev_id).name);
    EXPECT_EQ(false, st.midi_devices->at(dev_id).enabled);
}

TEST_F(midi_control_middleware_test, remove_and_readd_enabled_device)
{
    using testing::_;
    using testing::ReturnRef;
    using testing::WhenDynamicCastTo;

    auto prev_dev_id = midi::device_id_t::generate();
    auto next_dev_id = midi::device_id_t::generate();

    midi_control_middleware sut(
            make_middleware_functors(mf_mock),
            [=]() -> std::vector<midi::device_update> {
                return {midi::device_removed{.device_id = prev_dev_id},
                        midi::device_added{
                                .device_id = next_dev_id,
                                .name = "test"}};
            });

    state st;

    st.midi_inputs = std::vector<midi::device_id_t>({prev_dev_id});
    st.midi_devices = midi_devices_t{
            {prev_dev_id, midi_device_config{.name = "test", .enabled = true}}};

    actions::activate_midi_device activate_action;
    activate_action.device_id = next_dev_id;

    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));
    EXPECT_CALL(
            mf_mock,
            dispatch(WhenDynamicCastTo<actions::activate_midi_device const&>(
                    activate_action)));
    EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
        st = a.reduce(st);
    });
    actions::refresh_midi_devices action;
    sut(action);

    EXPECT_EQ(std::vector<midi::device_id_t>({next_dev_id}), *st.midi_inputs);
    EXPECT_EQ(
            (midi_devices_t{
                    {next_dev_id,
                     midi_device_config{.name = "test", .enabled = false}}}),
            *st.midi_devices);
}

TEST_F(midi_control_middleware_test, remove_eanabled_and_add_new_device)
{
    using testing::_;
    using testing::ReturnRef;
    using testing::WhenDynamicCastTo;

    auto prev_dev_id = midi::device_id_t::generate();
    auto next_dev_id = midi::device_id_t::generate();

    midi_control_middleware sut(
            make_middleware_functors(mf_mock),
            [=]() -> std::vector<midi::device_update> {
                return {midi::device_removed{.device_id = prev_dev_id},
                        midi::device_added{
                                .device_id = next_dev_id,
                                .name = "test2"}};
            });

    state st;

    st.midi_inputs = std::vector<midi::device_id_t>({prev_dev_id});
    st.midi_devices = midi_devices_t{
            {prev_dev_id, midi_device_config{.name = "test", .enabled = true}}};

    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));
    EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
        st = a.reduce(st);
    });

    actions::refresh_midi_devices action;
    sut(action);

    EXPECT_EQ(std::vector<midi::device_id_t>({next_dev_id}), *st.midi_inputs);
    EXPECT_EQ(
            (midi_devices_t{
                    {next_dev_id,
                     midi_device_config{.name = "test2", .enabled = false}}}),
            *st.midi_devices);
}

} // namespace piejam::runtime::test
