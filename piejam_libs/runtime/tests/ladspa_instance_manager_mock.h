// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/ladspa/instance_manager.h>

#include <piejam/entity_id.h>

#include <gmock/gmock.h>

namespace piejam::runtime::test
{

struct ladspa_instance_manager_mock : public ladspa::instance_manager
{
    MOCK_METHOD(ladspa::instance_id, load, (ladspa::plugin_descriptor const&));
    MOCK_METHOD(void, unload, (ladspa::instance_id const&));
    MOCK_METHOD(
            std::span<ladspa::port_descriptor const>,
            control_inputs,
            (ladspa::instance_id const&),
            (const));
};

} // namespace piejam::runtime::test
