// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/fx/ladspa_control.h>

#include <piejam/entity_id.h>

#include <gmock/gmock.h>

namespace piejam::runtime::test
{

struct ladspa_fx_control_mock : public fx::ladspa_control
{
    MOCK_METHOD(
            fx::ladspa_instance_id,
            load,
            (audio::ladspa::plugin_descriptor const&));
    MOCK_METHOD(void, unload, (fx::ladspa_instance_id const&));
    MOCK_METHOD(
            std::span<audio::ladspa::port_descriptor const>,
            control_inputs,
            (fx::ladspa_instance_id const&),
            (const));
};

} // namespace piejam::runtime::test
