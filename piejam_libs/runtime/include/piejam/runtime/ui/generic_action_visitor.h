#pragma once

#include <piejam/runtime/ui/action.h>

#include <piejam/redux/middleware_functors.h>

namespace piejam::runtime::ui
{

template <class State, class Data>
class generic_action_visitor
{
public:
    using action_t = ui::action<State>;
    using mw_fs_t = redux::middleware_functors<State, action_t>;

    generic_action_visitor(mw_fs_t const& mw_fs, Data& data)
        : m_mw_fs{mw_fs}
        , data{data}
    {
    }

    auto get_state() const -> State const& { return m_mw_fs.get_state(); }
    void dispatch(action_t const& a) { m_mw_fs.dispatch(a); }
    void next(action_t const& a) { m_mw_fs.next(a); }

private:
    mw_fs_t const& m_mw_fs;

public:
    Data& data;
};

} // namespace piejam::runtime::ui
