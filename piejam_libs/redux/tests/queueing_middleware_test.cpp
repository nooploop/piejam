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

#include <piejam/redux/queueing_middleware.h>

#include <gtest/gtest.h>

#include <vector>

namespace piejam::redux::test
{

struct queaction
{
    int x;

    auto clone() const -> std::unique_ptr<queaction>
    {
        return std::make_unique<queaction>(queaction{x});
    }
};

TEST(queueing_middleware, if_currently_not_dispatching_proceed_to_next)
{
    int result{};
    queueing_middleware<queaction> sut(
            [&result](queaction const& a) { result = a.x; });

    sut.operator()(queaction{7});

    EXPECT_EQ(7, result);
}

TEST(queueing_middleware,
     if_currently_dispatching_action_is_queued_and_executed_after_the_current_one)
{
    std::vector<int> result;
    queueing_middleware<queaction> sut(
            [dispatch = bool{true}, &result, &sut](queaction const& a) mutable {
                if (dispatch)
                {
                    sut.operator()(queaction{58});
                    dispatch = false;
                }

                result.push_back(a.x);
            });

    sut.operator()(queaction{23});

    ASSERT_EQ(2u, result.size());
    EXPECT_EQ(23, result[0]);
    EXPECT_EQ(58, result[1]);
}

} // namespace piejam::redux::test
