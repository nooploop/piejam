// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "middleware_functors_mock.h"

#include <piejam/midi/device_update.h>
#include <piejam/runtime/actions/activate_midi_device.h>
#include <piejam/runtime/actions/refresh_midi_devices.h>
#include <piejam/runtime/actions/request_info_update.h>
#include <piejam/runtime/actions/request_parameters_update.h>
#include <piejam/runtime/actions/save_app_config.h>
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
    struct some_action final : public reducible_action
    {
        [[nodiscard]] auto clone() const -> std::unique_ptr<action> override
        {
            return std::make_unique<some_action>();
        }

        void reduce(state&) const override
        {
        }
    };

    midi_control_middleware sut({});

    some_action action;
    EXPECT_CALL(mf_mock, next(testing::Ref(action)));
    sut(make_middleware_functors(mf_mock), action);
}

TEST_F(midi_control_middleware_test,
       for_refresh_midi_devices_action_and_no_updates_no_action_is_propagated)
{
    midi_control_middleware sut({});

    actions::refresh_midi_devices action;
    sut(make_middleware_functors(mf_mock), action);
}

TEST_F(midi_control_middleware_test, refresh_midi_devices_and_add_update)
{
    using testing::_;

    auto dev_id = midi::device_id_t::generate();
    midi_control_middleware sut(

            [dev_id]() -> std::vector<midi::device_update> {
                return {midi::device_added{
                        .device_id = dev_id,
                        .name = "test"}};
            });

    state st;

    EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
        dynamic_cast<reducible_action const&>(a).reduce(st);
    });
    actions::refresh_midi_devices action;
    sut(make_middleware_functors(mf_mock), action);

    ASSERT_EQ(1u, st.midi_inputs->size());
    EXPECT_EQ(dev_id, st.midi_inputs->at(0));

    ASSERT_TRUE(st.midi_devices->contains(dev_id));
    EXPECT_EQ("test", *st.midi_devices->at(dev_id).name);
    EXPECT_EQ(false, st.midi_devices->at(dev_id).enabled);
}

TEST_F(midi_control_middleware_test,
       refresh_midi_devices_and_add_and_remove_update)
{
    using testing::_;
    using testing::ReturnRef;

    auto dev_id = midi::device_id_t::generate();
    midi_control_middleware sut(

            [dev_id]() -> std::vector<midi::device_update> {
                return {midi::device_added{.device_id = dev_id, .name = "test"},
                        midi::device_removed{.device_id = dev_id}};
            });

    state st;

    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));
    EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
        dynamic_cast<reducible_action const&>(a).reduce(st);
    });
    actions::refresh_midi_devices action;
    sut(make_middleware_functors(mf_mock), action);

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

            [dev_id]() -> std::vector<midi::device_update> {
                return {midi::device_added{.device_id = dev_id, .name = "test"},
                        midi::device_removed{
                                .device_id = midi::device_id_t::generate()}};
            });

    state st;

    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));
    EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
        dynamic_cast<reducible_action const&>(a).reduce(st);
    });
    actions::refresh_midi_devices action;
    sut(make_middleware_functors(mf_mock), action);

    ASSERT_EQ(1u, st.midi_inputs->size());
    EXPECT_EQ(dev_id, st.midi_inputs->at(0));

    ASSERT_TRUE(st.midi_devices->contains(dev_id));
    EXPECT_EQ("test", *st.midi_devices->at(dev_id).name);
    EXPECT_EQ(false, st.midi_devices->at(dev_id).enabled);
}

TEST_F(midi_control_middleware_test, remove_and_readd_enabled_device)
{
    using testing::_;
    using testing::Field;
    using testing::ReturnRef;
    using testing::WhenDynamicCastTo;

    using namespace std::string_literals;

    auto prev_dev_id = midi::device_id_t::generate();
    auto next_dev_id = midi::device_id_t::generate();

    midi_control_middleware sut([=]() -> std::vector<midi::device_update> {
        return {midi::device_removed{.device_id = prev_dev_id},
                midi::device_added{.device_id = next_dev_id, .name = "test"}};
    });

    state st;

    st.midi_inputs = std::vector<midi::device_id_t>({prev_dev_id});
    st.midi_devices = midi_devices_t{
            {prev_dev_id,
             midi_device_config{.name = box("test"s), .enabled = true}}};

    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));
    EXPECT_CALL(
            mf_mock,
            dispatch(WhenDynamicCastTo<actions::activate_midi_device const&>(
                    Field(&actions::activate_midi_device::device_id,
                          next_dev_id))));
    EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
        dynamic_cast<reducible_action const&>(a).reduce(st);
    });
    actions::refresh_midi_devices action;
    sut(make_middleware_functors(mf_mock), action);

    EXPECT_EQ(std::vector<midi::device_id_t>({next_dev_id}), *st.midi_inputs);

    auto midi_device = st.midi_devices->find(next_dev_id);
    ASSERT_NE(midi_device, st.midi_devices->end());
    EXPECT_EQ(midi_device->second.name, "test"s);
    EXPECT_FALSE(midi_device->second.enabled);
}

TEST_F(midi_control_middleware_test, remove_eanabled_and_add_new_device)
{
    using testing::_;
    using testing::ReturnRef;
    using testing::WhenDynamicCastTo;

    using namespace std::string_literals;

    auto prev_dev_id = midi::device_id_t::generate();
    auto next_dev_id = midi::device_id_t::generate();

    midi_control_middleware sut([=]() -> std::vector<midi::device_update> {
        return {midi::device_removed{.device_id = prev_dev_id},
                midi::device_added{.device_id = next_dev_id, .name = "test2"}};
    });

    state st;

    st.midi_inputs = std::vector<midi::device_id_t>({prev_dev_id});
    st.midi_devices = midi_devices_t{
            {prev_dev_id,
             midi_device_config{.name = box("test"s), .enabled = true}}};

    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));
    EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
        dynamic_cast<reducible_action const&>(a).reduce(st);
    });

    actions::refresh_midi_devices action;
    sut(make_middleware_functors(mf_mock), action);

    EXPECT_EQ(std::vector<midi::device_id_t>({next_dev_id}), *st.midi_inputs);

    auto midi_device = st.midi_devices->find(next_dev_id);
    ASSERT_NE(midi_device, st.midi_devices->end());
    EXPECT_EQ(midi_device->second.name, "test2"s);
    EXPECT_FALSE(midi_device->second.enabled);
}

TEST_F(midi_control_middleware_test,
       save_app_config_is_populated_with_currently_enabled_devices)
{
    using testing::_;
    using testing::ElementsAre;
    using testing::Field;
    using testing::ReturnRef;
    using testing::WhenDynamicCastTo;

    using namespace std::string_literals;

    midi_control_middleware sut(
            [=]() -> std::vector<midi::device_update> { return {}; });

    state st;

    st.midi_devices = midi_devices_t{
            {midi::device_id_t::generate(),
             midi_device_config{.name = box("test"s), .enabled = true}}};

    actions::save_app_config action("save_file");
    ASSERT_TRUE(action.enabled_midi_devices.empty());

    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));
    EXPECT_CALL(
            mf_mock,
            next(WhenDynamicCastTo<actions::save_app_config const&>(
                    Field(&actions::save_app_config::enabled_midi_devices,
                          ElementsAre(std::string("test"))))));

    sut(make_middleware_functors(mf_mock), action);
}

TEST_F(midi_control_middleware_test,
       save_app_config_is_populated_with_previously_enabled_devices)
{
    using testing::_;
    using testing::ElementsAre;
    using testing::Field;
    using testing::ReturnRef;
    using testing::WhenDynamicCastTo;

    auto dev_id = midi::device_id_t::generate();

    midi_control_middleware sut([=]() -> std::vector<midi::device_update> {
        return {midi::device_removed{.device_id = dev_id}};
    });

    state st;
    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));

    st.midi_inputs = std::vector<midi::device_id_t>({dev_id});
    st.midi_devices = midi_devices_t{
            {dev_id,
             midi_device_config{
                     .name = {std::in_place, "test"},
                     .enabled = true}}};

    // setup, we need to remove the enabled device first
    {
        EXPECT_CALL(mf_mock, next(_)).WillOnce([&st](auto const& a) {
            dynamic_cast<reducible_action const&>(a).reduce(st);
        });

        actions::refresh_midi_devices action;
        sut(make_middleware_functors(mf_mock), action);
    }

    ASSERT_TRUE(st.midi_inputs->empty());
    ASSERT_TRUE(st.midi_devices->empty());

    {
        actions::save_app_config action("save_file");
        ASSERT_TRUE(action.enabled_midi_devices.empty());

        EXPECT_CALL(
                mf_mock,
                next(WhenDynamicCastTo<actions::save_app_config const&>(
                        Field(&actions::save_app_config::enabled_midi_devices,
                              ElementsAre(std::string("test"))))));

        sut(make_middleware_functors(mf_mock), action);
    }
}

TEST_F(midi_control_middleware_test,
       request_info_update_will_request_parameter_updates_for_each_midi_assignment)
{
    using testing::_;
    using testing::Field;
    using testing::InSequence;
    using testing::ReturnRef;
    using testing::WhenDynamicCastTo;

    midi_control_middleware sut(
            [=]() -> std::vector<midi::device_update> { return {}; });

    state st;
    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));

    auto param_id = float_parameter_id::generate();
    st.midi_assignments = midi_assignments_map{{param_id, midi_assignment{}}};

    {
        InSequence seq;

        EXPECT_CALL(
                mf_mock,
                next(WhenDynamicCastTo<actions::request_info_update const&>(
                        _)));

        actions::request_parameters_update::parameter_ids_t expected_ids;
        std::get<0>(expected_ids).emplace(param_id);

        EXPECT_CALL(
                mf_mock,
                next(WhenDynamicCastTo<
                        actions::request_parameters_update const&>(
                        Field(&actions::request_parameters_update::param_ids,
                              expected_ids))));

        actions::request_info_update action;
        sut(make_middleware_functors(mf_mock), action);
    }
}

} // namespace piejam::runtime::test
