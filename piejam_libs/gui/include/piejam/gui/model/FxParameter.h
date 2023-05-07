// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxParameterKeyId.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

namespace piejam::gui::model
{

class FxParameter final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(double value READ value NOTIFY valueChanged FINAL)
    Q_PROPERTY(QString valueString READ valueString NOTIFY valueStringChanged
                       FINAL)

    Q_PROPERTY(bool stepped READ stepped NOTIFY steppedChanged FINAL)
    Q_PROPERTY(double minValue READ minValue NOTIFY minValueChanged FINAL)
    Q_PROPERTY(double maxValue READ maxValue NOTIFY maxValueChanged FINAL)

    Q_PROPERTY(bool isSwitch READ isSwitch NOTIFY isSwitchChanged FINAL)
    Q_PROPERTY(
            bool switchValue READ switchValue NOTIFY switchValueChanged FINAL)

    Q_PROPERTY(
            piejam::gui::model::MidiAssignable* midi READ midi CONSTANT FINAL)

public:
    FxParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            piejam::gui::model::FxParameterKeyId const&);
    ~FxParameter();

    auto name() const noexcept -> QString const&
    {
        return m_name;
    }

    void setName(QString const& x)
    {
        if (m_name != x)
        {
            m_name = x;
            emit nameChanged();
        }
    }

    auto value() const noexcept -> double
    {
        return m_value;
    }

    void setValue(double x)
    {
        if (m_value != x)
        {
            m_value = x;
            emit valueChanged();
        }
    }

    Q_INVOKABLE void changeValue(double);

    auto valueString() const noexcept -> QString const&
    {
        return m_valueString;
    }

    void setValueString(QString const& x)
    {
        if (m_valueString != x)
        {
            m_valueString = x;
            emit valueStringChanged();
        }
    }

    auto stepped() const noexcept -> bool
    {
        return m_stepped;
    }

    void setStepped(bool x)
    {
        if (m_stepped != x)
        {
            m_stepped = x;
            emit steppedChanged();
        }
    }

    auto minValue() const noexcept -> double
    {
        return m_minValue;
    }

    void setMinValue(double x)
    {
        if (m_minValue != x)
        {
            m_minValue = x;
            emit minValueChanged();
        }
    }

    auto maxValue() const noexcept -> double
    {
        return m_maxValue;
    }

    void setMaxValue(double x)
    {
        if (m_maxValue != x)
        {
            m_maxValue = x;
            emit maxValueChanged();
        }
    }

    Q_INVOKABLE QString intValueToString(int value);

    auto isSwitch() const noexcept -> bool
    {
        return m_isSwitch;
    }

    void setIsSwitch(bool x)
    {
        if (m_isSwitch != x)
        {
            m_isSwitch = x;
            emit isSwitchChanged();
        }
    }

    auto switchValue() const noexcept -> bool
    {
        return m_switchValue;
    }

    void setSwitchValue(bool x)
    {
        if (m_switchValue != x)
        {
            m_switchValue = x;
            emit switchValueChanged();
        }
    }

    Q_INVOKABLE void changeSwitchValue(bool);

    auto midi() const -> MidiAssignable*;

signals:
    void nameChanged();
    void valueChanged();
    void valueStringChanged();
    void steppedChanged();
    void minValueChanged();
    void maxValueChanged();
    void isSwitchChanged();
    void switchValueChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QString m_name;
    double m_value{};
    QString m_valueString;

    bool m_stepped{};
    double m_minValue{};
    double m_maxValue{1.0};

    bool m_isSwitch{};
    bool m_switchValue{};
};

} // namespace piejam::gui::model
