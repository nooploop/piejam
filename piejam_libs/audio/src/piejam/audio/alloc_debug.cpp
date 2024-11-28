// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/alloc_debug.h>

#include <cstddef>
#include <cstdlib>
#include <new>

#ifndef NDEBUG

static thread_local bool s_prohibit_dynamic_memory_allocation = false;

auto
operator new(std::size_t count) -> void*
{
    if (!s_prohibit_dynamic_memory_allocation)
    {
        if (count == 0)
        {
            ++count; // avoid std::malloc(0) which may return nullptr on success
        }

        if (void* ptr = std::malloc(count))
        {
            return ptr;
        }
    }

    throw std::bad_alloc{};
}

auto
operator new(std::size_t count, std::align_val_t al) -> void*
{
    if (!s_prohibit_dynamic_memory_allocation)
    {
        if (count == 0)
        {
            ++count; // avoid std::malloc(0) which may return nullptr on success
        }

        if (void* ptr = std::aligned_alloc(static_cast<std::size_t>(al), count))
        {
            return ptr;
        }
    }

    throw std::bad_alloc{};
}

void
operator delete(void* ptr) noexcept
{
    std::free(ptr);
}

void
operator delete(void* ptr, std::size_t /*sz*/) noexcept
{
    std::free(ptr);
}

void
operator delete(void* ptr, std::align_val_t /*al*/) noexcept
{
    std::free(ptr);
}

#endif

namespace piejam::audio
{

void
prohibit_dynamic_memory_allocation()
{
#ifndef NDEBUG
    s_prohibit_dynamic_memory_allocation = true;
#endif
}

} // namespace piejam::audio
