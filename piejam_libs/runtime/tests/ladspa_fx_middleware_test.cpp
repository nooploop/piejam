// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ladspa_instance_manager_mock.h"
#include "middleware_functors_mock.h"

#include <piejam/ladspa/port_descriptor.h>
#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/ladspa_fx_middleware.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/action.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace piejam::runtime::test
{

struct ladspa_fx_middleware_test : testing::Test
{
    testing::StrictMock<middleware_functors_mock> mf_mock;
    testing::StrictMock<ladspa_instance_manager_mock> lfx_ctrl_mock;
};

TEST_F(ladspa_fx_middleware_test, unknown_action_is_passed_to_next)
{
    struct some_action final : public action
    {
        [[nodiscard]] auto clone() const -> std::unique_ptr<action> override
        {
            return std::make_unique<some_action>();
        }

        [[nodiscard]] auto reduce(state const& st) const -> state
        {
            return st;
        }
    };

    ladspa_fx_middleware sut(lfx_ctrl_mock);

    some_action action;
    EXPECT_CALL(mf_mock, next(testing::Ref(action)));
    sut(make_middleware_functors(mf_mock), action);
}

TEST_F(ladspa_fx_middleware_test,
       produce_missing_ladspa_fx_action_if_plugin_id_not_found_in_registry)
{
    using testing::Field;
    using testing::ReturnRef;
    using testing::WhenDynamicCastTo;

    ladspa_fx_middleware sut(lfx_ctrl_mock);

    state st;
    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));

    ladspa::plugin_id_t const plug_id{23};

    EXPECT_CALL(
            mf_mock,
            next(WhenDynamicCastTo<
                    actions::insert_missing_ladspa_fx_module const&>(Field(
                    &actions::insert_missing_ladspa_fx_module::
                            unavailable_ladspa,
                    Field(&fx::unavailable_ladspa::plugin_id, plug_id)))));

    actions::load_ladspa_fx_plugin action;
    action.plugin_id = plug_id;
    sut(make_middleware_functors(mf_mock), action);
}

TEST_F(ladspa_fx_middleware_test,
       produce_missing_ladspa_fx_action_if_plugin_could_not_be_loaded)
{
    using testing::Field;
    using testing::ReturnRef;
    using testing::WhenDynamicCastTo;

    ladspa_fx_middleware sut(lfx_ctrl_mock);

    ladspa::plugin_id_t const plug_id{23};

    state st;
    ladspa::plugin_descriptor plugin_desc{.id = plug_id};
    st.fx_registry.entries = std::vector<fx::registry::item>{plugin_desc};
    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));

    EXPECT_CALL(lfx_ctrl_mock, load(plugin_desc));

    EXPECT_CALL(
            mf_mock,
            next(WhenDynamicCastTo<
                    actions::insert_missing_ladspa_fx_module const&>(Field(
                    &actions::insert_missing_ladspa_fx_module::
                            unavailable_ladspa,
                    Field(&fx::unavailable_ladspa::plugin_id, plug_id)))));

    actions::load_ladspa_fx_plugin action;
    action.plugin_id = plug_id;
    sut(make_middleware_functors(mf_mock), action);
}

TEST_F(ladspa_fx_middleware_test,
       produce_insert_ladspa_fx_action_on_successful_plugin_load)
{
    using testing::Field;
    using testing::Return;
    using testing::ReturnRef;
    using testing::WhenDynamicCastTo;

    ladspa_fx_middleware sut(lfx_ctrl_mock);

    ladspa::plugin_id_t const plug_id{23};

    state st;
    ladspa::plugin_descriptor plugin_desc{.id = plug_id};
    st.fx_registry.entries = std::vector<fx::registry::item>{plugin_desc};
    EXPECT_CALL(mf_mock, get_state()).WillRepeatedly(ReturnRef(st));

    auto instance_id = ladspa::instance_id::generate();
    EXPECT_CALL(lfx_ctrl_mock, load(plugin_desc)).WillOnce(Return(instance_id));
    EXPECT_CALL(lfx_ctrl_mock, control_inputs(instance_id));
    EXPECT_CALL(
            mf_mock,
            next(WhenDynamicCastTo<actions::insert_ladspa_fx_module const&>(
                    Field(&actions::insert_ladspa_fx_module::instance_id,
                          instance_id))));

    actions::load_ladspa_fx_plugin action;
    action.plugin_id = plug_id;
    sut(make_middleware_functors(mf_mock), action);
}

} // namespace piejam::runtime::test
