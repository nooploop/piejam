// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/FxStream.h>

namespace piejam::app::gui::model
{

FxStream::FxStream(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        FxStreamKeyId const&)
    : Subscribable(store_dispatch, state_change_subscriber)
{
}

void
FxStream::onSubscribe()
{
}

} // namespace piejam::app::gui::model
