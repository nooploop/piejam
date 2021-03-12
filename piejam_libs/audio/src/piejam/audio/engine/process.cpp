// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/process.h>

#include <piejam/audio/engine/dag_executor.h>

#include <boost/assert.hpp>

namespace piejam::audio::engine
{

namespace
{

class dummy_dag_executor final : public dag_executor
{
public:
    void operator()(std::size_t) override {}
};

} // namespace

process::process()
    : m_executor(std::make_unique<dummy_dag_executor>())
{
}

process::~process()
{
    BOOST_ASSERT(!m_next_executor);
    delete m_next_executor.load();
}

bool
process::swap_executor(std::unique_ptr<dag_executor> next_dag_executor)
{
    if (!next_dag_executor)
        next_dag_executor = std::make_unique<dummy_dag_executor>();

    m_prev_executor = {};
    auto prev_executor_future = m_prev_executor.get_future();

    m_next_executor.store(
            next_dag_executor.release(),
            std::memory_order_release);

    auto const status =
            prev_executor_future.wait_for(std::chrono::milliseconds(200));

    if (status == std::future_status::ready)
    {
        prev_executor_future.get();
        return true;
    }
    else
    {
        delete m_next_executor.exchange(nullptr, std::memory_order_acq_rel);
        return false;
    }
}

void
process::operator()(std::size_t const buffer_size) noexcept
{
    if (auto next_dag_executor = std::unique_ptr<dag_executor>(
                m_next_executor.exchange(nullptr, std::memory_order_acq_rel)))
    {
        std::swap(m_executor, next_dag_executor);
        m_prev_executor.set_value(std::move(next_dag_executor));
    }

    (*m_executor)(buffer_size);
}

} // namespace piejam::audio::engine
