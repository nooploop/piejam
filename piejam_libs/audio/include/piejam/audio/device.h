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

#pragma once

#include <piejam/audio/process_function.h>
#include <piejam/thread/fwd.h>

namespace piejam::audio
{

class device
{
public:
    virtual ~device() = default;

    virtual bool is_open() const noexcept = 0;
    virtual void close() = 0;

    virtual bool is_running() const noexcept = 0;
    virtual void start(thread::configuration const&, process_function) = 0;
    virtual void stop() = 0;

    virtual auto cpu_load() const noexcept -> float = 0;
    virtual auto xruns() const noexcept -> std::size_t = 0;
};

class dummy_device final : public device
{
public:
    bool is_open() const noexcept override { return false; }
    void close() override {}

    bool is_running() const noexcept override { return false; }
    void start(thread::configuration const&, process_function) override {}
    void stop() override {}

    auto cpu_load() const noexcept -> float override { return 0.f; }
    auto xruns() const noexcept -> std::size_t override { return 0; }
};

} // namespace piejam::audio
