// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

namespace piejam::gui::model
{

class FxBrowser final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* entries READ entries CONSTANT)

public:
    FxBrowser(runtime::store_dispatch, runtime::subscriber&);
    ~FxBrowser();

    auto entries() noexcept -> FxBrowserList*;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
