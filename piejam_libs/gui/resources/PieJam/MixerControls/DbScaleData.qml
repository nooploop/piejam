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

import QtQuick 2.13

import "../Util/DbConvert.js" as DbConvert
import "../Util/MathExt.js" as MathExt

QtObject {
    id: root

    default property list<DbScaleTick> ticks

    function dbAt(position) {
        if (ticks.length === 0)
            throw "DbScale: Atleast one tick is required."

        if (ticks.length === 1) {
            var t = ticks[0]
            if (position < t.position) {
                return Number.NEGATIVE_INFINITY
            } else if (position === t.position) {
                return t.db
            } else {
                return Number.POSITIVE_INFINITY
            }
        } else {
            var min = ticks[0]
            var max = ticks[ticks.length - 1]

            if (position < min.position) {
                return Number.NEGATIVE_INFINITY
            } else if (position === max.position) {
                return max.db
            } else if (position > max.position) {
                return Number.POSITIVE_INFINITY
            } else {
                for (var i = 0; i < ticks.length - 1; ++i) {
                    var lo = ticks[i]
                    var hi = ticks[i + 1]
                    console.assert(lo.position < hi.position, "ticks is not sorted");
                    console.assert(lo.db < hi.db, "ticks is not sorted");

                    if (lo.position <= position && position < hi.position) {
                        if (lo.db === Number.NEGATIVE_INFINITY)
                            return Number.NEGATIVE_INFINITY
                        else if (hi.db === Number.POSITIVE_INFINITY)
                            return Number.POSITIVE_INFINITY
                        else
                            return MathExt.mapTo(position, lo.position, hi.position, lo.db, hi.db)
                    }
                }
            }
        }
    }

    function dbToPosition(db) {
        console.assert(ticks.length > 0, "Atleast one tick is required.")

        if (ticks.length === 1) {
            var t = ticks[0]
            if (db < t.db) {
                return 0
            } else if (db === t.db) {
                return t.position
            } else {
                return 1
            }
        } else {
            var min = ticks[0]
            var max = ticks[ticks.length - 1]

            if (db < min.db) {
                return 0
            } else if (db === max.db) {
                return max.position
            } else if (db > max.db) {
                return 1
            } else {
                for (var i = 0; i < ticks.length - 1; ++i) {
                    var lo = ticks[i]
                    var hi = ticks[i + 1]
                    console.assert(lo.position <= hi.position, "ticks is not sorted");
                    console.assert(lo.db <= hi.db, "ticks is not sorted");

                    if (lo.db <= db && db < hi.db) {
                        if (lo.db === Number.NEGATIVE_INFINITY) {
                            return lo.position
                        } else if (hi.db === Number.POSITIVE_INFINITY) {
                            return hi.position
                        } else {
                            return MathExt.mapTo(db, lo.db, hi.db, lo.position, hi.position)
                        }
                    }
                }
            }
        }
    }
}
