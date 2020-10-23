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

#include <boost/assert.hpp>

#include <algorithm>
#include <stdexcept>

namespace piejam::audio::engine
{

dag::dag(std::size_t queue_size)
{
    m_run_queue.reserve(queue_size);
}

auto
dag::add_task(task_t t) -> task_id_t
{
    auto id = m_free_id++;
    m_nodes[id].task = std::move(t);
    return id;
}

auto
dag::add_child_task(task_id_t parent, task_t t) -> task_id_t
{
    auto it = m_nodes.find(parent);

    BOOST_ASSERT_MSG(it != m_nodes.end(), "parent node not found");

    auto id = m_free_id++;
    m_nodes[id].task = std::move(t);
    m_nodes[id].num_parents += 1;
    it->second.children.push_back(std::ref(m_nodes[id]));
    return id;
}

void
dag::add_child(task_id_t parent, task_id_t child)
{
    auto it_parent = m_nodes.find(parent);

    BOOST_ASSERT_MSG(it_parent != m_nodes.end(), "parent node not found");

    auto it_child = m_nodes.find(child);

    BOOST_ASSERT_MSG(it_child != m_nodes.end(), "child node not found");

    BOOST_ASSERT_MSG(
            !is_descendent(it_child->second, it_parent->second),
            "child is ancestor of the parent");

    it_parent->second.children.push_back(it_child->second);
    it_child->second.num_parents += 1;
}

bool
dag::is_descendent(node const& parent, node const& descendent)
{
    if (&parent == &descendent)
        return true;

    return std::ranges::any_of(
            parent.children,
            [&descendent](node const& child) {
                return is_descendent(child, descendent);
            });
}

void
dag::operator()(thread_context const& ctx)
{
    BOOST_ASSERT(m_run_queue.empty());

    for (auto& id_node : m_nodes)
    {
        id_node.second.parents_to_process = id_node.second.num_parents;

        BOOST_ASSERT(m_run_queue.size() < m_run_queue.capacity());
        if (id_node.second.num_parents == 0)
            m_run_queue.push_back(std::addressof(id_node.second));
    }

    while (!m_run_queue.empty())
    {
        node* n = m_run_queue.back();
        m_run_queue.pop_back();
        BOOST_ASSERT(n->parents_to_process == 0);

        n->task(ctx);

        for (node& child : n->children)
        {
            if (1 == child.parents_to_process--)
            {
                BOOST_ASSERT(m_run_queue.size() < m_run_queue.capacity());
                m_run_queue.push_back(std::addressof(child));
            }
        }
    }
}

} // namespace piejam::audio::engine
