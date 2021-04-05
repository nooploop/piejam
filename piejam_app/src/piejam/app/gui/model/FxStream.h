// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/FxStreamKeyId.h>
#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/AudioStreamProvider.h>

#include <memory>

namespace piejam::app::gui::model
{

class FxStream final
    : public Subscribable<piejam::gui::model::AudioStreamProvider>
{
public:
    FxStream(
            runtime::store_dispatch,
            runtime::subscriber&,
            FxStreamKeyId const&);
    ~FxStream();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::app::gui::model
