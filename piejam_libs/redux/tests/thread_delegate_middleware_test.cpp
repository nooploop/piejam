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

#include <piejam/redux/thread_delegate_middleware.h>

#include <gtest/gtest.h>

#include <future>
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

TEST(thread_delegate_middleware,
     if_action_comes_on_required_thread_proceed_to_next)
{
    int next_result{};

    auto factory = make_thread_delegate_middleware(
            std::this_thread::get_id(),
            [](auto&&) { FAIL(); });
    auto sut = factory(
            []() { FAIL(); },
            [](queaction const&) { FAIL(); },
            redux::next_f<queaction>(
                    [&next_result](queaction const& a) { next_result = a.x; }));

    sut(queaction{23});

    EXPECT_EQ(23, next_result);
}

TEST(thread_delegate_middleware,
     if_action_comes_on_not_required_thread_delegate_to_required_one)
{
    int dispatch_result{};

    auto factory = make_thread_delegate_middleware(
            std::this_thread::get_id(),
            [](auto&& f) { f(); });
    auto sut = factory(
            []() { FAIL(); },
            [&dispatch_result](queaction const& a) { dispatch_result = a.x; },
            redux::next_f<queaction>([](queaction const&) { FAIL(); }));

    auto f = std::async(std::launch::async, [&sut]() { sut(queaction{23}); });
    f.wait();

    EXPECT_EQ(23, dispatch_result);
}

} // namespace piejam::redux::test
