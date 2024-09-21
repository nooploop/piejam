// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/system/file_utils.h>

#include <sstream>

namespace piejam::system
{

auto
make_unique_filename(
        std::filesystem::path const& base_dir,
        std::string_view base_filename,
        std::string_view extension) -> std::filesystem::path
{
    auto full_filename =
            base_dir /
            (std::stringstream{} << base_filename << '.' << extension).str();

    std::size_t file_num{};
    while (std::filesystem::exists(full_filename))
    {
        full_filename =
                base_dir / (std::stringstream{} << base_filename << " ("
                                                << std::to_string(++file_num)
                                                << ")." << extension)
                                   .str();
    }

    return full_filename;
}

} // namespace piejam::system
