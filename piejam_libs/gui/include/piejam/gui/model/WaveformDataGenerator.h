// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/gui/model/BusType.h>
#include <piejam/gui/model/StereoChannel.h>
#include <piejam/gui/model/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class WaveformDataGenerator final : public AudioStreamListener
{
    Q_OBJECT

public:
    WaveformDataGenerator();
    ~WaveformDataGenerator() override;

    void setStreamType(piejam::gui::model::BusType streamType);
    void setSamplesPerLine(int x);
    void setActive(bool active);
    void setStereoChannel(StereoChannel);

    void clear();
    void update(Stream const&) override;

signals:
    void generated(piejam::gui::model::WaveformData const&);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
