// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/outcome/std_result.hpp>

#include <filesystem>
#include <span>
#include <system_error>

namespace outcome = boost::outcome_v2;

namespace piejam::system
{

class device
{
public:
    device() noexcept = default;
    device(std::filesystem::path const& pathname);
    device(device const&) = delete;
    device(device&& other) noexcept;

    ~device();

    auto operator=(device const&) -> device& = delete;
    auto operator=(device&& other) noexcept -> device&;

    explicit operator bool() const noexcept
    {
        return m_fd != invalid;
    }

    [[nodiscard]] auto ioctl(unsigned long request) noexcept -> std::error_code;

    template <class T>
    [[nodiscard]] auto ioctl(unsigned long request, T& x) noexcept
            -> std::error_code
    {
        return ioctl(request, &x, sizeof(T));
    }

    [[nodiscard]] auto read(std::span<std::byte> buffer) noexcept
            -> outcome::std_result<std::size_t>;

    [[nodiscard]] auto set_nonblock(bool set = true) -> std::error_code;

private:
    [[nodiscard]] auto
    ioctl(unsigned long request, void* p, std::size_t size) noexcept
            -> std::error_code;

    static constexpr int invalid = -1;

    int m_fd{invalid};
};

template <>
auto device::ioctl(unsigned long request, device&) noexcept
        -> std::error_code = delete;

template <>
[[nodiscard]] auto
device::ioctl(unsigned long request, device const& other) noexcept
        -> std::error_code;

} // namespace piejam::system
