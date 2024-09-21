// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <span>
#include <vector>

namespace piejam::gui::model
{

class ScopeData : public QObject
{
    Q_OBJECT
public:
    using Samples = std::span<float const>;

    auto get() const noexcept -> Samples
    {
        return m_samples;
    }

    void set(Samples samples)
    {
        m_samples = std::vector(samples.begin(), samples.end());
        emit changed();
    }

    void clear()
    {
        m_samples.clear();
        emit changed();
    }

signals:
    void changed();

private:
    std::vector<float> m_samples;
};

} // namespace piejam::gui::model
