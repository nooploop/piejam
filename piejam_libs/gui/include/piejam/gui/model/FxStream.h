// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamProvider.h>
#include <piejam/gui/model/StreamId.h>
#include <piejam/gui/model/Subscribable.h>

#include <piejam/pimpl.h>

namespace piejam::gui::model
{

class FxStream final : public Subscribable<AudioStreamProvider>
{
public:
    FxStream(runtime::store_dispatch, runtime::subscriber&, StreamId const&);

private:
    void onSubscribe() override;

    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
