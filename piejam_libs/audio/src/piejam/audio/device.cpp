// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/device.h>

namespace piejam::audio
{

namespace
{

class dummy_device final : public device
{
public:
    [[nodiscard]] auto is_open() const noexcept -> bool override
    {
        return false;
    }

    void close() override
    {
    }

    [[nodiscard]] auto is_running() const noexcept -> bool override
    {
        return false;
    }

    void
    start(thread::configuration const&,
          init_process_function const&,
          process_function) override
    {
    }

    void stop() override
    {
    }

    [[nodiscard]] auto cpu_load() const noexcept -> float override
    {
        return 0.f;
    }

    [[nodiscard]] auto xruns() const noexcept -> std::size_t override
    {
        return 0;
    }
};

} // namespace

auto
make_dummy_device() -> std::unique_ptr<device>
{
    return std::make_unique<dummy_device>();
}

} // namespace piejam::audio
