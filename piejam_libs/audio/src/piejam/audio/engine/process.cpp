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
    void operator()(std::size_t) {}
};

} // namespace

process::process(
        unsigned num_device_input_channels,
        unsigned num_device_output_channels)
    : m_input_proc(num_device_input_channels)
    , m_output_proc(num_device_output_channels)
    , m_executor(std::make_unique<dummy_dag_executor>())
{
}

process::~process()
{
    BOOST_ASSERT(!m_next_executor);
    delete m_next_executor.load();
}

void
process::swap_executor(std::unique_ptr<dag_executor> next_dag_executor)
{
    if (!next_dag_executor)
        next_dag_executor = std::make_unique<dummy_dag_executor>();

    m_prev_executor = {};
    auto prev_executor_future = m_prev_executor.get_future();

    m_next_executor.store(
            next_dag_executor.release(),
            std::memory_order_release);

    prev_executor_future.get();
}

void
process::operator()(
        range::table_view<float const> const& in_audio,
        range::table_view<float> const& out_audio) noexcept
{
    m_input_proc.set_input(in_audio);
    m_output_proc.set_output(out_audio);

    if (auto next_dag_executor = std::unique_ptr<dag_executor>(
                m_next_executor.exchange(nullptr, std::memory_order_acq_rel)))
    {
        std::swap(m_executor, next_dag_executor);
        m_prev_executor.set_value(std::move(next_dag_executor));
    }

    (*m_executor)(in_audio.minor_size());
}

} // namespace piejam::audio::engine
