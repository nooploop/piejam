#pragma once

#include <filesystem>
#include <string_view>

namespace piejam::system
{

[[nodiscard]] auto make_unique_filename(
        std::filesystem::path base_dir,
        std::string_view base_filename,
        std::string_view extension) -> std::filesystem::path;

} // namespace piejam::system
