// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/UISettings.h>

#include <piejam/gui/model/StringList.h>

#include <piejam/algorithm/index_of.h>
#include <piejam/runtime/actions/ui_settings_actions.h>
#include <piejam/runtime/selectors.h>

#include <boost/assert.hpp>
#include <boost/polymorphic_cast.hpp>

namespace piejam::gui::model
{

namespace
{

constexpr auto display_rotations = std::array{0uz, 90uz, 180uz, 270uz};

} // namespace

UISettings::UISettings(runtime::state_access const& state_access)
    : CompositeSubscribableModel{state_access}
    , m_displayRotations{&addQObject<StringList>()}
{
    auto& displayRotations =
        boost::polymorphic_downcast<StringList&>(*m_displayRotations);
    displayRotations.append("0");
    displayRotations.append("90");
    displayRotations.append("180");
    displayRotations.append("270");
}

void
UISettings::onSubscribe()
{
    observe(
        runtime::selectors::select_display_rotation,
        [this](std::size_t displayRotation) {
            auto index =
                algorithm::index_of(display_rotations, displayRotation);
            if (index != algorithm::npos)
            {
                setDisplayRotation(static_cast<int>(index));
            }
        });

    observe(
        runtime::selectors::select_on_screen_keyboard_enabled,
        [this](bool enabled) { setOnScreenKeyboardEnabled(enabled); });
}

void
UISettings::selectDisplayRotation(unsigned index)
{
    BOOST_ASSERT(index < display_rotations.size());

    runtime::actions::set_display_rotation action;
    action.display_rotation = display_rotations[index];
    dispatch(action);
}

void
UISettings::toggleOnScreenKeyboardEnabled(bool enabled)
{
    runtime::actions::set_on_screen_keyboard_enabled action;
    action.enabled = enabled;
    dispatch(action);
}

} // namespace piejam::gui::model
