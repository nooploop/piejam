// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/entity_id.h>
#include <piejam/midi/device_manager.h>
#include <piejam/midi/input_event_handler.h>
#include <piejam/runtime/midi_input_controller.h>

namespace piejam::runtime
{

namespace
{

struct default_midi_input_controller final : public midi_input_controller
{
    default_midi_input_controller(midi::device_manager& midi_device_manager)
        : m_midi_device_manager(midi_device_manager)
    {
    }

    auto activate_input_device(midi::device_id_t const id) -> bool override
    {
        return m_midi_device_manager.activate_input_device(id);
    }

    auto deactivate_input_device(midi::device_id_t const id) -> void override
    {
        m_midi_device_manager.deactivate_input_device(id);
    }

    auto make_input_event_handler()
            -> std::unique_ptr<midi::input_event_handler> override
    {
        return m_midi_device_manager.make_input_event_handler();
    }

private:
    midi::device_manager& m_midi_device_manager;
};

struct dummy_midi_input_controller final : public midi_input_controller
{
    auto activate_input_device(midi::device_id_t) -> bool override
    {
        return false;
    }

    auto deactivate_input_device(midi::device_id_t) -> void override
    {
    }

    auto make_input_event_handler()
            -> std::unique_ptr<midi::input_event_handler> override
    {
        return nullptr;
    }
};

} // namespace

auto
make_midi_input_controller(midi::device_manager& midi_device_manager)
        -> std::unique_ptr<midi_input_controller>
{
    return std::make_unique<default_midi_input_controller>(midi_device_manager);
}

auto
make_dummy_midi_input_controller() -> std::unique_ptr<midi_input_controller>
{
    return std::make_unique<dummy_midi_input_controller>();
}

} // namespace piejam::runtime
