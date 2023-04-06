#pragma once

#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/generic_action_visitor.h>

namespace piejam::runtime
{

template <class Data>
using generic_action_visitor = ui::generic_action_visitor<state, Data>;

} // namespace piejam::runtime
