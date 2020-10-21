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

#include <functional>
#include <unordered_map>
#include <vector>

namespace piejam::audio::engine
{

//! Simple dependency graph executor. No multi-threading support yet.
class dag
{
public:
    using task_id_t = std::size_t;
    using task_t = std::function<void()>;

    dag(std::size_t run_queue_size);
    dag(dag const&) = delete;
    dag(dag&&) = default;

    auto operator=(dag const&) -> dag& = delete;
    auto operator=(dag &&) -> dag& = default;

    auto add_task(task_t) -> task_id_t;
    auto add_child_task(task_id_t parent, task_t) -> task_id_t;
    void add_child(task_id_t parent, task_id_t child);

    void operator()();

private:
    struct node;
    using node_ref = std::reference_wrapper<node>;

    struct node
    {
        std::size_t parents_to_process{};
        std::size_t num_parents{};
        task_t task;
        std::vector<node_ref> children;
    };

    static bool is_descendent(node const& parent, node const& descendent);

    std::size_t m_free_id{};
    std::unordered_map<task_id_t, node> m_nodes;
    std::vector<node*> m_run_queue;
};

} // namespace piejam::audio::engine
