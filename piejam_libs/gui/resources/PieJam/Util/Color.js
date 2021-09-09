// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

.pragma library

function setAlpha(color, alpha) {
    return Qt.rgba(color.r, color.g, color.b, alpha)
}
