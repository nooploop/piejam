// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/parameter/map.h>

#include <gtest/gtest.h>

namespace piejam::runtime::fx::test
{

TEST(fx_parameter_id, less_than)
{
    float_parameters float_params;

    auto id1 = float_params.add(float_parameter{});
    auto id2 = float_params.add(float_parameter{});

    EXPECT_LT(parameter_id(id1), parameter_id(id2));
}

} // namespace piejam::runtime::fx::test
