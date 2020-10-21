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

#include <piejam/audio/engine/graph_to_dag.h>

#include <piejam/audio/engine/dag.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/period_sizes.h>
#include <piejam/functional/address_compare.h>

#include <boost/assert.hpp>

#include <array>
#include <map>
#include <memory>
#include <set>
#include <span>
#include <vector>

namespace piejam::audio::engine
{

namespace
{

class processor_job final
{
public:
    using output_buffer_t = std::array<float, max_period_size>;

    processor_job(processor& proc, std::size_t const& buffer_size_ref)
        : m_proc(proc)
        , m_buffer_size(buffer_size_ref)
        , m_output_buffers(m_proc.num_outputs(), output_buffer_t{})
        , m_inputs(m_proc.num_inputs(), empty_result_ref())
        , m_outputs(m_proc.num_outputs())
        , m_results(m_proc.num_outputs())
    {
        std::ranges::copy(m_output_buffers, m_outputs.begin());
    }

    auto result_ref(std::size_t index) -> std::span<float const>&
    {
        return m_results[index];
    }

    void connect_result(std::size_t index, std::span<float const>& res)
    {
        BOOST_ASSERT(index < m_inputs.size());
        m_inputs[index] = std::ref(res);
    }

    void operator()()
    {
        // set output buffer sizes
        for (auto& out : m_outputs)
            out = {out.data(), m_buffer_size};

        // initialize results with output buffers
        std::ranges::copy(m_outputs, m_results.begin());

        m_proc.process(m_inputs, m_outputs, m_results);
    }

private:
    static auto empty_result_ref()
            -> std::reference_wrapper<std::span<float const> const>
    {
        static std::span<float const> res;
        return std::cref(res);
    }

    processor& m_proc;
    std::size_t const& m_buffer_size;
    std::vector<output_buffer_t> m_output_buffers;

    std::vector<std::reference_wrapper<std::span<float const> const>> m_inputs;
    std::vector<std::span<float>> m_outputs;
    std::vector<std::span<float const>> m_results;
};

} // namespace

auto
graph_to_dag(
        graph const& g,
        std::size_t const run_queue_size,
        std::size_t const& buffer_size_ref) -> dag
{
    dag result(run_queue_size);

    std::map<
            std::reference_wrapper<processor>,
            std::pair<dag::task_id_t, processor_job*>,
            address_less<processor>>
            processor_job_mapping;

    auto add_job = [&](graph::endpoint const& e) {
        auto job = std::make_shared<processor_job>(e.proc, buffer_size_ref);
        auto job_ptr = job.get();
        auto id = result.add_task([j = std::move(job)]() { (*j)(); });
        processor_job_mapping.emplace(e.proc, std::pair(id, job_ptr));
    };

    // create a job for each processor
    for (auto const& [src, dst] : g.wires())
    {
        if (!processor_job_mapping.count(src.proc))
            add_job(src);

        if (!processor_job_mapping.count(dst.proc))
            add_job(dst);
    }

    // connect jobs according to graph wires
    std::set<std::pair<dag::task_id_t, dag::task_id_t>> added_deps;
    for (auto const& [src, dst] : g.wires())
    {
        auto const& [src_id, src_job] = processor_job_mapping[src.proc];
        auto const& [dst_id, dst_job] = processor_job_mapping[dst.proc];

        if (!added_deps.count({src_id, dst_id}))
            result.add_child(src_id, dst_id);

        dst_job->connect_result(dst.port, src_job->result_ref(src.port));
    }

    return result;
}

} // namespace piejam::audio::engine
