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

class FxIntParameter : public FxParameter
{
    Q_OBJECT

    Q_PROPERTY(int value READ value NOTIFY valueChanged FINAL)
    Q_PROPERTY(int minValue READ minValue NOTIFY minValueChanged FINAL)
    Q_PROPERTY(int maxValue READ maxValue NOTIFY maxValueChanged FINAL)

public:
    FxIntParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            piejam::gui::model::FxParameterKeyId const&);
    ~FxIntParameter();

    static inline constexpr auto StaticType = Type::Int;

    auto value() const noexcept -> int
    {
        return m_value;
    }

    Q_INVOKABLE void changeValue(int);

    auto minValue() const noexcept -> double
    {
        return m_minValue;
    }

    auto maxValue() const noexcept -> double
    {
        return m_maxValue;
    }

signals:
    void valueChanged();
    void minValueChanged();
    void maxValueChanged();

private:
    void onSubscribe() override;

    void setValue(int x)
    {
        if (m_value != x)
        {
            m_value = x;
            emit valueChanged();
        }
    }

    void setMinValue(double x)
    {
        if (m_minValue != x)
        {
            m_minValue = x;
            emit minValueChanged();
        }
    }

    void setMaxValue(double x)
    {
        if (m_maxValue != x)
        {
            m_maxValue = x;
            emit maxValueChanged();
        }
    }

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    int m_value{};
    int m_minValue{};
    int m_maxValue{1};
};

} // namespace piejam::gui::model
