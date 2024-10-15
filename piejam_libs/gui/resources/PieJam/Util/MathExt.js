// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

.pragma library

function mapTo(x, srcLo, srcHi, dstLo, dstHi) {
    return ((x - srcLo) / (srcHi - srcLo)) * (dstHi - dstLo) + dstLo
}

function clamp(x, min, max) {
    return Math.min(Math.max(x, min), max)
}

function toRad(deg) {
    return (deg / 180) * Math.PI
}

function toDeg(rad) {
    return (rad / Math.PI) * 180
}

function fromNormalized(v, dstLo, dstHi)
{
    return v * (dstHi - dstLo) + dstLo
}

function toNormalized(v, srcLo, srcHi)
{
    return (v - srcLo) / (srcHi - srcLo)
}
