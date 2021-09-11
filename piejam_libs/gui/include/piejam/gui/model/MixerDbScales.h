// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/fwd.h>

#include <QObject>

#include <memory>

namespace piejam::gui::model
{

class MixerDbScales : public QObject
{
    Q_OBJECT
    Q_PROPERTY(piejam::gui::model::DbScaleData* levelMeterScale READ
                       levelMeterScale CONSTANT)
    Q_PROPERTY(piejam::gui::model::DbScaleData* volumeFaderScale READ
                       volumeFaderScale CONSTANT)
public:
    MixerDbScales();
    ~MixerDbScales();

    auto levelMeterScale() const noexcept -> DbScaleData*
    {
        return m_levelMeterScale.get();
    }

    auto volumeFaderScale() const noexcept -> DbScaleData*
    {
        return m_volumeFaderScale.get();
    }

private:
    std::unique_ptr<DbScaleData> m_levelMeterScale;
    std::unique_ptr<DbScaleData> m_volumeFaderScale;
};

extern MixerDbScales g_mixerDbScales;

} // namespace piejam::gui::model
