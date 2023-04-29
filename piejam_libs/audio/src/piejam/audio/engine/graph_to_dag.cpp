// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/graph_to_dag.h>

#include <piejam/audio/engine/dag.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/processor_job.h>
#include <piejam/audio/engine/slice.h>
#include <piejam/audio/period_size.h>
#include <piejam/functional/address_compare.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <array>
#include <initializer_list>
#include <map>
#include <memory>
#include <ranges>
#include <set>
#include <span>
#include <vector>

namespace piejam::audio::engine
{

auto
graph_to_dag(graph const& g) -> dag
{
    dag result;

    std::map<
            std::reference_wrapper<processor>,
            std::pair<dag::task_id_t, processor_job*>,
            decltype(address_less<processor>)>
            processor_job_mapping;

    std::vector<processor_job*> clear_event_buffer_jobs;

    auto add_job = [&](graph_endpoint const& e) {
        auto job = std::make_shared<processor_job>(e.proc);
        auto job_ptr = job.get();
        auto id = result.add_task(
                [j = std::move(job)](thread_context const& ctx) { (*j)(ctx); });
        processor_job_mapping.emplace(e.proc, std::pair(id, job_ptr));
        if (!e.proc.get().event_outputs().empty())
        {
            clear_event_buffer_jobs.push_back(job_ptr);
        }
    };

    // create a job for each processor
    for (auto const& [src, dst] : g.audio)
    {
        if (!processor_job_mapping.count(src.proc))
        {
            add_job(src);
        }

        if (!processor_job_mapping.count(dst.proc))
        {
            add_job(dst);
        }
    }

    for (auto const& [src, dst] : g.event)
    {
        if (!processor_job_mapping.count(src.proc))
        {
            add_job(src);
        }

        if (!processor_job_mapping.count(dst.proc))
        {
            add_job(dst);
        }
    }

    // connect jobs according to audio wires
    std::set<std::pair<dag::task_id_t, dag::task_id_t>> added_deps;
    for (auto const& [src, dst] : g.audio)
    {
        auto const& [src_id, src_job] = processor_job_mapping[src.proc];
        auto const& [dst_id, dst_job] = processor_job_mapping[dst.proc];

        if (!added_deps.count({src_id, dst_id}))
        {
            result.add_child(src_id, dst_id);
            added_deps.emplace(src_id, dst_id);
        }

        dst_job->connect_result(dst.port, src_job->result_ref(src.port));
    }

    // connect jobs according to event wires
    for (auto const& [src, dst] : g.event)
    {
        auto const& [src_id, src_job] = processor_job_mapping[src.proc];
        auto const& [dst_id, dst_job] = processor_job_mapping[dst.proc];

        if (!added_deps.count({src_id, dst_id}))
        {
            result.add_child(src_id, dst_id);
            added_deps.emplace(src_id, dst_id);
        }

        dst_job->connect_event_result(
                dst.port,
                src_job->event_result_ref(src.port));
    }

    // if we have processors with event outputs, we need to clear their
    // buffers as last step
    if (!clear_event_buffer_jobs.empty())
    {
        // get final jobs first, before inserting the clear job
        auto const final_jobs = [&]() {
            std::vector<dag::task_id_t> jobs_without_children;
            for (auto const& [id, children] : result.graph())
            {
                if (children.empty())
                {
                    jobs_without_children.push_back(id);
                }
            }
            return jobs_without_children;
        }();

        auto clear_job_id =
                result.add_task([jobs = std::move(clear_event_buffer_jobs)](
                                        thread_context const&) {
                    for (auto job : jobs)
                    {
                        job->clear_event_output_buffers();
                    }
                });

        for (dag::task_id_t const final_job_id : final_jobs)
        {
            result.add_child(final_job_id, clear_job_id);
        }
    }

    return result;
}

} // namespace piejam::audio::engine
