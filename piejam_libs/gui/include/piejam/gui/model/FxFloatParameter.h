// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxParameter.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class FxFloatParameter : public FxParameter
{
    Q_OBJECT

    Q_PROPERTY(double value READ value NOTIFY valueChanged FINAL)

public:
    FxFloatParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            piejam::gui::model::FxParameterKeyId const&);
    ~FxFloatParameter();

    auto value() const noexcept -> double
    {
        return m_value;
    }

    Q_INVOKABLE void changeValue(double);

signals:
    void valueChanged();

private:
    void onSubscribe() override;

    void setValue(double x)
    {
        if (m_value != x)
        {
            m_value = x;
            emit valueChanged();
        }
    }

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    double m_value{};
};

} // namespace piejam::gui::model
