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

#include <piejam/audio/engine/dag.h>

#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/thread_context.h>
#include <piejam/thread/job_deque.h>
#include <piejam/thread/worker.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <atomic>
#include <iostream>
#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>

namespace piejam::audio::engine
{

namespace
{

class dag_executor_base : public dag_executor
{
protected:
    dag_executor_base(dag::tasks_t const& tasks, dag::graph_t const& graph)
        : m_nodes(make_nodes(tasks, graph))
    {
    }

    struct node;
    using node_ref = std::reference_wrapper<node>;

    struct node
    {
        std::atomic_size_t parents_to_process{};
        std::size_t num_parents{};
        dag::task_t task;
        std::vector<node_ref> children;
    };

    using nodes_t = std::unordered_map<dag::task_id_t, node>;

    nodes_t m_nodes;

private:
    static auto make_nodes(dag::tasks_t const& tasks, dag::graph_t const& graph)
            -> nodes_t
    {
        nodes_t nodes;
        for (auto const& [id, task] : tasks)
            nodes[id].task = task;

        for (auto const& [parent_id, children] : graph)
        {
            BOOST_ASSERT(nodes.count(parent_id));
            std::ranges::transform(
                    children,
                    std::back_inserter(nodes[parent_id].children),
                    [&nodes](dag::task_id_t const child_id) {
                        BOOST_ASSERT(nodes.count(child_id));
                        return std::ref(nodes[child_id]);
                    });

            for (dag::task_id_t const child_id : children)
                ++nodes[child_id].num_parents;
        }

        return nodes;
    }
};

class dag_executor_st final : public dag_executor_base
{
public:
    dag_executor_st(dag::tasks_t const& tasks, dag::graph_t const& graph)
        : dag_executor_base(tasks, graph)
    {
        m_run_queue.reserve(m_nodes.size());
    }

    void operator()() override
    {
        for (auto& id_node : m_nodes)
        {
            id_node.second.parents_to_process = id_node.second.num_parents;

            if (id_node.second.num_parents == 0)
            {
                BOOST_ASSERT(m_run_queue.size() < m_run_queue.capacity());
                m_run_queue.push_back(std::addressof(id_node.second));
            }
        }

        while (!m_run_queue.empty())
        {
            node* const n = m_run_queue.back();
            m_run_queue.pop_back();

            BOOST_ASSERT(
                    n->parents_to_process.load(std::memory_order_relaxed) == 0);
            n->task(m_thread_context);

            for (node& child : n->children)
            {
                if (1 == child.parents_to_process.fetch_sub(
                                 1,
                                 std::memory_order_relaxed))
                {
                    BOOST_ASSERT(m_run_queue.size() < m_run_queue.capacity());
                    m_run_queue.push_back(std::addressof(child));
                }
            }
        }

        m_event_memory.release();
    }

private:
    audio::engine::event_buffer_memory m_event_memory{1u << 20};
    audio::engine::thread_context m_thread_context{
            &m_event_memory.memory_resource()};
    std::vector<node*> m_run_queue;
};

class dag_executor_mt final : public dag_executor_base
{
public:
    dag_executor_mt(
            dag::tasks_t const& tasks,
            dag::graph_t const& graph,
            std::span<thread::configuration const> const& wt_configs)
        : dag_executor_base(tasks, graph)
        , m_run_queues(1 + wt_configs.size())
        , m_main_worker(0, m_nodes_to_process, m_run_queues)
        , m_workers(make_workers(wt_configs, m_nodes_to_process, m_run_queues))
    {
    }

    void operator()() override
    {
        BOOST_ASSERT(m_run_queues[0].size() == 0);

        m_nodes_to_process.store(m_nodes.size(), std::memory_order_release);

        for (auto& rq : m_run_queues)
            rq.reset();

        distribute_initial_tasks();

        for (auto const& wt : m_workers)
            wt->wakeup();

        m_main_worker();
    }

private:
    void distribute_initial_tasks()
    {
        std::size_t next_queue{1 % m_run_queues.size()};
        for (auto& id_node : m_nodes)
        {
            id_node.second.parents_to_process = id_node.second.num_parents;

            if (id_node.second.num_parents == 0)
            {
                BOOST_ASSERT(
                        m_run_queues[next_queue].size() <
                        m_run_queues[next_queue].capacity());
                m_run_queues[next_queue].push(std::addressof(id_node.second));
                next_queue = (next_queue + 1) % m_run_queues.size();
            }
        }
    }

    using workers_t = std::vector<std::unique_ptr<thread::worker>>;

    struct dag_worker
    {
        dag_worker(
                std::size_t const worker_index,
                std::atomic_size_t& nodes_to_process,
                std::span<thread::job_deque<node>> run_queues)
            : m_worker_index(worker_index)
            , m_nodes_to_process(nodes_to_process)
            , m_run_queues(std::move(run_queues))
        {
        }

        void operator()()
        {
            while (m_nodes_to_process.load(std::memory_order_relaxed))
            {
                if (node* n = m_run_queues[m_worker_index].pop())
                {
                    while (n)
                        n = process_node(*n);
                }
                else
                {
                    std::size_t const num_queues = m_run_queues.size();
                    for (std::size_t i = 1; i < num_queues; ++i)
                    {
                        std::size_t const steal_index =
                                (m_worker_index + i) % num_queues;
                        if (node* n = m_run_queues[steal_index].steal())
                        {
                            while (n)
                                n = process_node(*n);
                            break;
                        }
                    }
                }
            }

            m_event_memory.release();
        }

    private:
        auto process_node(node& n) -> node*
        {
            thread::job_deque<node>& run_queue = m_run_queues[m_worker_index];

            BOOST_ASSERT(n.parents_to_process == 0);

            n.task(m_thread_context);

            m_nodes_to_process.fetch_sub(1, std::memory_order_acq_rel);

            node* next{};
            for (node& child : n.children)
            {
                if (1 == child.parents_to_process.fetch_sub(
                                 1,
                                 std::memory_order_acq_rel))
                {
                    if (next)
                    {
                        BOOST_ASSERT(run_queue.size() < run_queue.capacity());
                        run_queue.push(std::addressof(child));
                    }
                    else
                    {
                        next = std::addressof(child);
                    }
                }
            }

            return next;
        }

        std::size_t m_worker_index;
        audio::engine::event_buffer_memory m_event_memory{1u << 20};
        audio::engine::thread_context m_thread_context{
                &m_event_memory.memory_resource()};
        std::atomic_size_t& m_nodes_to_process;
        std::span<thread::job_deque<node>> m_run_queues;
    };

    static auto make_workers(
            std::span<thread::configuration const> const& wt_configs,
            std::atomic_size_t& nodes_to_process,
            std::span<thread::job_deque<node>> run_queues) -> workers_t
    {
        workers_t workers;
        workers.reserve(wt_configs.size());

        for (std::size_t i = 0; i < wt_configs.size(); ++i)
        {
            workers.emplace_back(std::make_unique<thread::worker>(
                    dag_worker(i + 1, nodes_to_process, run_queues),
                    wt_configs[i]));
        }

        return workers;
    }

    std::atomic_size_t m_nodes_to_process{};
    std::vector<thread::job_deque<node>> m_run_queues;
    dag_worker m_main_worker;
    workers_t m_workers;
};

bool
is_descendent(
        dag::graph_t const& t,
        dag::task_id_t const parent,
        dag::task_id_t const descendent)
{
    if (parent == descendent)
        return true;

    return std::ranges::any_of(
            t.at(parent),
            [&t, descendent](dag::task_id_t const child) {
                return is_descendent(t, child, descendent);
            });
}

} // namespace

dag::dag() = default;

auto
dag::add_task(task_t t) -> task_id_t
{
    auto id = m_free_id++;
    m_tasks.emplace_back(id, std::move(t));
    m_graph[id];
    return id;
}

auto
dag::add_child_task(task_id_t const parent, task_t t) -> task_id_t
{
    auto it = m_graph.find(parent);

    BOOST_ASSERT_MSG(it != m_graph.end(), "parent node not found");

    auto id = add_task(std::move(t));
    it->second.push_back(id);
    return id;
}

void
dag::add_child(task_id_t const parent, task_id_t const child)
{
    auto it_parent = m_graph.find(parent);

    BOOST_ASSERT_MSG(it_parent != m_graph.end(), "parent node not found");

    auto it_child = m_graph.find(child);

    BOOST_ASSERT_MSG(it_child != m_graph.end(), "child node not found");

    BOOST_ASSERT_MSG(
            !is_descendent(m_graph, it_child->first, it_parent->first),
            "child is ancestor of the parent");

    it_parent->second.push_back(it_child->first);
}

auto
dag::make_runnable(std::span<thread::configuration const> const& wt_configs)
        -> std::unique_ptr<dag_executor>
{
    if (wt_configs.empty())
        return std::make_unique<dag_executor_st>(m_tasks, m_graph);
    else
        return std::make_unique<dag_executor_mt>(m_tasks, m_graph, wt_configs);
}

} // namespace piejam::audio::engine
