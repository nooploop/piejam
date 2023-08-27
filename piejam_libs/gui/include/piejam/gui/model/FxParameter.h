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

class FxParameter : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(Type type READ type CONSTANT FINAL)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(QString valueString READ valueString NOTIFY valueStringChanged
                       FINAL)
    Q_PROPERTY(
            piejam::gui::model::MidiAssignable* midi READ midi CONSTANT FINAL)

public:
    FxParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            piejam::gui::model::FxParameterKeyId const&);
    ~FxParameter();

    enum class Type
    {
        Float,
        Int,
        Bool,
        Enum,
    };

    Q_ENUM(Type)

    virtual auto type() const noexcept -> Type;

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

    auto midi() const -> MidiAssignable*;

    Q_INVOKABLE void resetToDefault();

signals:
    void nameChanged();
    void valueStringChanged();

protected:
    void onSubscribe() override;

    auto paramKeyId() const -> FxParameterKeyId;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QString m_name;
    QString m_valueString;
};

auto makeFxParameter(
        runtime::store_dispatch,
        runtime::subscriber&,
        piejam::gui::model::FxParameterKeyId const&)
        -> std::unique_ptr<FxParameter>;

} // namespace piejam::gui::model
