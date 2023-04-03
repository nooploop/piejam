#pragma once

#include <memory>

namespace piejam::redux
{

// clang-format off
using flag_resetter =
        std::unique_ptr<bool, decltype([](bool* flag) { *flag = false; })>;
// clang-format on

} // namespace piejam::redux
