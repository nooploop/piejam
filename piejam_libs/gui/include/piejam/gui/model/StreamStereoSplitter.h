// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamListener.h>

#include <memory>

namespace piejam::gui::model
{

class StreamStereoSplitter final : public AudioStreamListener
{
    Q_OBJECT
public:
    void update(Stream const&) override;

signals:
    void splitted(std::vector<Stream>);
};

} // namespace piejam::gui::model
