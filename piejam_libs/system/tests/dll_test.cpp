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

#include <piejam/system/dll.h>

#include <gtest/gtest.h>

namespace piejam::system::test
{

TEST(dll, open_dll_and_call_symbol)
{
    dll sut(PIEJAM_SYSTEM_TEST_DLL);
    auto f = sut.symbol<int()>("some_function");
    ASSERT_NE(nullptr, f);
    EXPECT_EQ(5, f());
}

TEST(dll, open_dll_twice_and_call_symbol)
{
    dll sut1(PIEJAM_SYSTEM_TEST_DLL);

    {
        dll sut2(PIEJAM_SYSTEM_TEST_DLL);

        auto f = sut2.symbol<int (*)()>("some_function");
        ASSERT_NE(nullptr, f);
        EXPECT_EQ(5, f());
    }

    auto f = sut1.symbol<int()>("some_function");
    ASSERT_NE(nullptr, f);
    EXPECT_EQ(5, f());
}

TEST(dll, open_non_existent_dll_will_throw_in_constructor)
{
    EXPECT_THROW(dll("foobar.dll"), std::system_error);
}

TEST(dll, getting_unknown_symbol_will_throw)
{
    dll sut(PIEJAM_SYSTEM_TEST_DLL);
    EXPECT_THROW(
            sut.symbol<void()>("function_which_doesnt_exist"),
            std::system_error);
}

} // namespace piejam::system::test
