// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

.pragma library

function to_dB(x) {
    return x === 0 ? Number.NEGATIVE_INFINITY : Math.log10(x) * 20
}

function from_dB(x) {
    return x === Number.NEGATIVE_INFINITY ? 0 : Math.pow(10, (x / 20))
}
