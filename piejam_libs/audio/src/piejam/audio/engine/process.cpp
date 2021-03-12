// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/process.h>

#include <piejam/audio/engine/dag_executor.h>
#include <piejam/range/indices.h>
#include <piejam/range/table_view.h>

#include <boost/assert.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/irange.hpp>

#include <ranges>

namespace piejam::audio::engine
{

namespace
{

class dummy_dag_executor final : public dag_executor
{
public:
    void operator()(std::size_t) override {}
};

template <class Processor>
auto
make_io_processors(std::size_t num_channels)
{
    auto names = boost::irange(num_channels) |
                 boost::adaptors::transformed(
                         [](std::size_t const i) { return std::to_string(i); });
    return std::vector<Processor>(names.begin(), names.end());
}

} // namespace

process::process(
        std::size_t num_device_input_channels,
        std::size_t num_device_output_channels)
    : m_input_procs(
              make_io_processors<input_processor>(num_device_input_channels))
    , m_output_procs(
              make_io_processors<output_processor>(num_device_output_channels))
    , m_executor(std::make_unique<dummy_dag_executor>())
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
process::operator()(
        std::span<pcm_input_buffer_converter const> const& in_converter,
        std::span<pcm_output_buffer_converter const> const& out_converter,
        std::size_t const buffer_size) noexcept
{
    if (auto next_dag_executor = std::unique_ptr<dag_executor>(
                m_next_executor.exchange(nullptr, std::memory_order_acq_rel)))
    {
        std::swap(m_executor, next_dag_executor);
        m_prev_executor.set_value(std::move(next_dag_executor));
    }

    BOOST_ASSERT(m_input_procs.size() == in_converter.size());
    for (std::size_t const i : range::indices(in_converter))
    {
        m_input_procs[i].set_input(in_converter[i]);
    }

    BOOST_ASSERT(m_output_procs.size() == out_converter.size());
    for (std::size_t const i : range::indices(out_converter))
    {
        m_output_procs[i].set_output(out_converter[i]);
    }

    (*m_executor)(buffer_size);
}

} // namespace piejam::audio::engine
