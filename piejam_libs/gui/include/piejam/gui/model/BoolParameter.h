// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Parameter.h>

#include <memory>

namespace piejam::gui::model
{

class BoolParameter final : public Parameter
{
    Q_OBJECT

    Q_PROPERTY(bool value READ value NOTIFY valueChanged FINAL)

public:
    BoolParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            piejam::gui::model::ParameterId const&);
    ~BoolParameter() override;

    static constexpr auto StaticType = Type::Bool;

    auto value() const noexcept -> bool
    {
        return m_value;
    }

    Q_INVOKABLE void changeValue(bool);

signals:
    void valueChanged();

private:
    void onSubscribe() override;

    void setValue(bool x)
    {
        if (m_value != x)
        {
            m_value = x;
            emit valueChanged();
        }
    }

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    bool m_value{};
};

} // namespace piejam::gui::model
