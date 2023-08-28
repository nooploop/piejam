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
    WaveformDataGenerator(std::span<BusType const> substreamConfigs);
    ~WaveformDataGenerator() override;

    void setSamplesPerPixel(int x);

    void setActive(std::size_t substreamIndex, bool active);
    void setChannel(std::size_t substreamIndex, StereoChannel);
    void setFreeze(bool);

    void clear();

    void update(AudioStream const&) override;

signals:
    void generated(std::span<piejam::gui::model::WaveformData const>);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
