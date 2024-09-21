// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>

#include <functional>
#include <mutex>

namespace piejam::log
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
    void sink_it_(spdlog::details::log_msg const& msg) override
    {
        m_sink_it(msg);
    }
    void flush_() override
    {
        m_flush();
    }

private:
    sink_it_fn_t m_sink_it;
    flush_fn_t m_flush;
};

using generic_log_sink_mt = generic_log_sink<std::mutex>;
using generic_log_sink_st = generic_log_sink<spdlog::details::null_mutex>;

} // namespace piejam::log
