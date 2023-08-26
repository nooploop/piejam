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
    Q_PROPERTY(double normalizedValue READ normalizedValue NOTIFY
                       normalizedValueChanged FINAL)

public:
    FxFloatParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            piejam::gui::model::FxParameterKeyId const&);
    ~FxFloatParameter();

    static inline constexpr auto StaticType = Type::Float;

    auto value() const noexcept -> double
    {
        return m_value;
    }

    auto normalizedValue() const noexcept -> double
    {
        return m_normalizedValue;
    }

    Q_INVOKABLE void changeNormalizedValue(double);

signals:
    void valueChanged();
    void normalizedValueChanged();

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

    void setNormalizedValue(double x)
    {
        if (m_normalizedValue != x)
        {
            m_normalizedValue = x;
            emit normalizedValueChanged();
        }
    }

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    double m_value{};
    double m_normalizedValue{};
};

} // namespace piejam::gui::model
