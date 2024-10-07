// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/ParameterId.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class Parameter : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(Type type READ type CONSTANT FINAL)
    M_PIEJAM_GUI_PROPERTY(QString, name, setName)
    M_PIEJAM_GUI_PROPERTY(QString, valueString, setValueString)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::MidiAssignable*, midi)

public:
    Parameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            piejam::gui::model::ParameterId const&);
    ~Parameter() override;

    auto paramId() const -> ParameterId;

    enum class Type
    {
        Float,
        Int,
        Bool,
        Enum,
    };

    Q_ENUM(Type)

    virtual auto type() const noexcept -> Type;

    Q_INVOKABLE void resetToDefault();

protected:
    void onSubscribe() override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

auto makeParameter(
        runtime::store_dispatch,
        runtime::subscriber&,
        piejam::gui::model::ParameterId const&) -> std::unique_ptr<Parameter>;

} // namespace piejam::gui::model
