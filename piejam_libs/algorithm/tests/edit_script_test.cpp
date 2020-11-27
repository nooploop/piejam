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

#include <piejam/algorithm/edit_script.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace piejam::algorithm::test
{

TEST(edit_script, insert_into_empty)
{
    std::string src;
    std::string dst("abcde");

    EXPECT_EQ(
            edit_script(src, dst),
            (std::vector<edit_script_op<char>>{
                    edit_script_insertion{0, 'a'},
                    edit_script_insertion{1, 'b'},
                    edit_script_insertion{2, 'c'},
                    edit_script_insertion{3, 'd'},
                    edit_script_insertion{4, 'e'},
            }));
}

TEST(edit_script, delete_all)
{
    std::string src("abcde");
    std::string dst;

    EXPECT_EQ(
            edit_script(src, dst),
            (std::vector<edit_script_op<char>>{
                    edit_script_deletion{0},
                    edit_script_deletion{1},
                    edit_script_deletion{2},
                    edit_script_deletion{3},
                    edit_script_deletion{4},
            }));
}

TEST(edit_script, add_at_the_end)
{
    std::string src("ab");
    std::string dst("abc");

    EXPECT_EQ(
            edit_script(src, dst),
            (std::vector<edit_script_op<char>>{
                    edit_script_insertion{2, 'c'},
            }));
}

TEST(edit_script, delete_at_the_end)
{
    std::string src("abc");
    std::string dst("ab");

    EXPECT_EQ(
            edit_script(src, dst),
            (std::vector<edit_script_op<char>>{
                    edit_script_deletion{2},
            }));
}

TEST(edit_script, delete_in_the_middle)
{
    std::string src("abc");
    std::string dst("ac");

    EXPECT_EQ(
            edit_script(src, dst),
            (std::vector<edit_script_op<char>>{
                    edit_script_deletion{1},
            }));
}

TEST(edit_script, delete_at_begin)
{
    std::string src("abc");
    std::string dst("bc");

    EXPECT_EQ(
            edit_script(src, dst),
            (std::vector<edit_script_op<char>>{
                    edit_script_deletion{0},
            }));
}

TEST(edit_script, replace_with_single)
{
    std::string src("abc");
    std::string dst("d");

    EXPECT_EQ(
            edit_script(src, dst),
            (std::vector<edit_script_op<char>>{
                    edit_script_deletion{0},
                    edit_script_deletion{1},
                    edit_script_deletion{2},
                    edit_script_insertion{0, 'd'},
            }));
}

TEST(edit_script, replace_with_multiple)
{
    std::string src("abc");
    std::string dst("def");

    EXPECT_EQ(
            edit_script(src, dst),
            (std::vector<edit_script_op<char>>{
                    edit_script_deletion{0},
                    edit_script_deletion{1},
                    edit_script_deletion{2},
                    edit_script_insertion{0, 'd'},
                    edit_script_insertion{1, 'e'},
                    edit_script_insertion{2, 'f'},
            }));
}

} // namespace piejam::algorithm::test
