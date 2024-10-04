// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

.pragma library

function findChild(parent, predicate) {
    var obj = null
    if (parent === null)
        return null
    var children = parent.children
    for (var i = 0; i < children.length; i++) {
        obj = children[i]
        if (predicate(obj)) {
            break;
        }
        obj = findChild(obj, predicate)
        if (obj)
            break
    }
    return obj
}
