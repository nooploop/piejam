// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <qobjectdefs.h>

#include <type_traits>

#define M_PIEJAM_GUI_PROPERTY(type, name, setterName)                          \
private:                                                                       \
    Q_PROPERTY(type name READ name NOTIFY name##Changed FINAL)                 \
    type m_##name{};                                                           \
                                                                               \
public:                                                                        \
    using name##_property_t = std::conditional_t<                              \
            std::is_trivially_copyable_v<type>,                                \
            type,                                                              \
            type const&>;                                                      \
                                                                               \
    Q_SIGNAL void name##Changed();                                             \
    auto name() const noexcept -> name##_property_t                            \
    {                                                                          \
        return m_##name;                                                       \
    }                                                                          \
                                                                               \
    void setterName(name##_property_t x)                                       \
    {                                                                          \
        if (m_##name != x)                                                     \
        {                                                                      \
            m_##name = x;                                                      \
            Q_EMIT name##Changed();                                            \
        }                                                                      \
    }                                                                          \
                                                                               \
private:

#define M_PIEJAM_GUI_READONLY_PROPERTY(type, name)                             \
private:                                                                       \
    Q_PROPERTY(type name READ name CONSTANT FINAL)                             \
                                                                               \
public:                                                                        \
    using name##_property_t = std::conditional_t<                              \
            std::is_trivially_copyable_v<type>,                                \
            type,                                                              \
            type const&>;                                                      \
                                                                               \
    auto name() const noexcept -> name##_property_t;                           \
                                                                               \
private:

#define M_PIEJAM_GUI_WRITABLE_PROPERTY(type, name, setterName)                 \
private:                                                                       \
    Q_PROPERTY(                                                                \
            type name READ name WRITE setterName NOTIFY name##Changed FINAL)   \
    type m_##name{};                                                           \
                                                                               \
public:                                                                        \
    using name##_property_t = std::conditional_t<                              \
            std::is_trivially_copyable_v<type>,                                \
            type,                                                              \
            type const&>;                                                      \
                                                                               \
    Q_SIGNAL void name##Changed();                                             \
    auto name() const noexcept -> name##_property_t                            \
    {                                                                          \
        return m_##name;                                                       \
    }                                                                          \
                                                                               \
    void setterName(name##_property_t x)                                       \
    {                                                                          \
        if (m_##name != x)                                                     \
        {                                                                      \
            m_##name = x;                                                      \
            Q_EMIT name##Changed();                                            \
        }                                                                      \
    }                                                                          \
                                                                               \
private:
