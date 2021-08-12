// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class FxChain final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* modules READ modules CONSTANT)

public:
    FxChain(runtime::store_dispatch, runtime::subscriber&);
    ~FxChain();

    auto modules() noexcept -> FxModulesList* { return &m_modules; }

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    FxModulesList m_modules;
};

} // namespace piejam::gui::model
