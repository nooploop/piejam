// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/parameters.h>

#include <memory>

namespace piejam::gui::model
{

class StereoLevelParameter : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(double levelLeft READ levelLeft NOTIFY levelLeftChanged FINAL)
    Q_PROPERTY(double levelRight READ levelRight NOTIFY levelRightChanged FINAL)

public:
    StereoLevelParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::stereo_level_parameter_id const&);
    ~StereoLevelParameter() override;

    auto levelLeft() const noexcept -> double
    {
        return m_levelLeft;
    }

    auto levelRight() const noexcept -> double
    {
        return m_levelRight;
    }

signals:
    void levelLeftChanged();
    void levelRightChanged();

private:
    void onSubscribe() override;

    void setLevel(double left, double right)
    {
        if (m_levelLeft != left)
        {
            m_levelLeft = left;
            emit levelLeftChanged();
        }

        if (m_levelRight != right)
        {
            m_levelRight = right;
            emit levelRightChanged();
        }
    }

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    double m_levelLeft{};
    double m_levelRight{};
};

} // namespace piejam::gui::model
