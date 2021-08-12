// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamProvider.h>
#include <piejam/gui/model/FxStreamKeyId.h>
#include <piejam/gui/model/Subscribable.h>

#include <memory>

namespace piejam::gui::model
{

class FxStream final : public Subscribable<AudioStreamProvider>
{
public:
    FxStream(
            runtime::store_dispatch,
            runtime::subscriber&,
            FxStreamKeyId const&);
    ~FxStream();

    void requestUpdate() override;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
