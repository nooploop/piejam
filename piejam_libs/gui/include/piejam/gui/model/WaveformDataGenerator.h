// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>

namespace piejam::gui::model
{

class WaveformDataGenerator final : public AudioStreamListener
{
    Q_OBJECT

public:
    explicit WaveformDataGenerator(std::span<BusType const> substreamConfigs);

    void setSamplesPerPixel(int x);

    void setActive(std::size_t substreamIndex, bool active);
    void setChannel(std::size_t substreamIndex, StereoChannel);
    void setFreeze(bool);

    void clear();

    void update(AudioStream) override;

signals:
    void generated(std::span<piejam::gui::model::WaveformData const>);

private:
    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
