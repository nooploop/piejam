// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

.pragma library

const twenty_div_log_10 = 20 / Math.log(10)

function linToDb(x) {
    var res = x === 0 ? Number.NEGATIVE_INFINITY : Math.log(x) * twenty_div_log_10
    return res
}

function dbToLin(x) {
    return x === Number.NEGATIVE_INFINITY ? 0 : Math.pow(10, (x / 20))
}

function dbToString(db) {
    if (db === Number.NEGATIVE_INFINITY)
        return "-Inf"
    else if (db === Number.POSITIVE_INFINITY)
        return "Inf"
    else if (db > 0)
        return "+%1".arg(db)
    else
        return "%1".arg(db)
}
