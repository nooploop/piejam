// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/FxParameterKeyId.h>
#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/FxParameter.h>

namespace piejam::app::gui::model
{

class FxParameter final : public Subscribable<piejam::gui::model::FxParameter>
{
public:
    FxParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            FxParameterKeyId const&);
    ~FxParameter();

    void changeValue(double) override;
    void changeSwitchValue(bool) override;

    auto midi() const -> piejam::gui::model::MidiAssignable* override;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::app::gui::model
