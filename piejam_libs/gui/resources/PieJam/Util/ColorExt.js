// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

.pragma library

function setAlpha(color, alpha) {
    return Qt.rgba(color.r, color.g, color.b, alpha)
}

function mapNormalized(n, from, to) {
    return Qt.rgba(
                from.r + n * (to.r - from.r),
                from.g + n * (to.g - from.g),
                from.b + n * (to.b - from.b),
                from.a + n * (to.a - from.a))
}
