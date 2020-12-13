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

#include <piejam/audio/engine/fwd.h>
#include <piejam/thread/fwd.h>

#include <functional>
#include <memory>
#include <span>
#include <unordered_map>
#include <vector>

namespace piejam::audio::engine
{

//! Directed acyclic graph of tasks.
class dag
{
public:
    using task_id_t = std::size_t;
    using task_t =
            std::function<void(thread_context const&, std::size_t buffer_size)>;
    using tasks_t = std::vector<std::pair<task_id_t, task_t>>;
    using graph_t = std::unordered_map<task_id_t, std::vector<task_id_t>>;

    dag();
    dag(dag const&) = delete;
    dag(dag&&) = default;

    auto operator=(dag const&) -> dag& = delete;
    auto operator=(dag&&) -> dag& = default;

    auto graph() const noexcept -> graph_t const& { return m_graph; }

    auto add_task(task_t) -> task_id_t;
    auto add_child_task(task_id_t parent, task_t) -> task_id_t;
    void add_child(task_id_t parent, task_id_t child);

    auto make_runnable(
            std::span<thread::worker> const& = {},
            std::size_t event_memory_size = (1u << 16))
            -> std::unique_ptr<dag_executor>;

private:
    std::size_t m_free_id{};
    graph_t m_graph;
    tasks_t m_tasks;
};

} // namespace piejam::audio::engine
