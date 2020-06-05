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

#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>

#include <functional>
#include <mutex>

namespace piejam::core
{

template <typename Mutex>
class generic_log_sink : public spdlog::sinks::base_sink<Mutex>
{
public:
    using sink_it_fn_t = std::function<void(spdlog::details::log_msg const&)>;
    using flush_fn_t = std::function<void()>;

    generic_log_sink(sink_it_fn_t sink_it_fn, flush_fn_t flush_fn)
        : m_sink_it(std::move(sink_it_fn))
        , m_flush(std::move(flush_fn))
    {
    }

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {
        m_sink_it(msg);
    }
    void flush_() override { m_flush(); }

private:
    sink_it_fn_t m_sink_it;
    flush_fn_t m_flush;
};

using generic_log_sink_mt = generic_log_sink<std::mutex>;
using generic_log_sink_st = generic_log_sink<spdlog::details::null_mutex>;

} // namespace piejam::core
