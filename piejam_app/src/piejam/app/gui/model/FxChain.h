// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/FxChain.h>

#include <memory>

namespace piejam::app::gui::model
{

class FxChain final : public Subscribable<piejam::gui::model::FxChain>
{
public:
    FxChain(runtime::store_dispatch, runtime::subscriber&);
    ~FxChain();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::app::gui::model
