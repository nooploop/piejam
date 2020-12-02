// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

.pragma library

function linToDb(x) {
    return x === 0 ? Number.NEGATIVE_INFINITY : (Math.log(x) / Math.log(10)) * 20
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
        return "+%1 dB".arg(db)
    else
        return "%1 dB".arg(db)
}
