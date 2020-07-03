// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <QObject>

namespace piejam::gui::model
{

class MixerChannel final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(double gain READ gain NOTIFY gainChanged FINAL)
    Q_PROPERTY(double levelLeft READ levelLeft NOTIFY levelLeftChanged FINAL)
    Q_PROPERTY(double levelRight READ levelRight NOTIFY levelRightChanged FINAL)
    Q_PROPERTY(double pan READ pan NOTIFY panChanged FINAL)
    Q_PROPERTY(double balance READ balance NOTIFY balanceChanged FINAL)
    Q_PROPERTY(double mono READ mono NOTIFY monoChanged FINAL)

public:
    MixerChannel(QObject* parent = nullptr);

    bool enabled() const noexcept { return m_enabled; }
    void setEnabled(bool);

    auto gain() const noexcept -> double { return m_gain; }
    void setGain(double);

    auto levelLeft() const noexcept -> double { return m_levelLeft; }
    auto levelRight() const noexcept -> double { return m_levelRight; }
    void setLevel(double left, double right);

    auto pan() const noexcept -> double { return m_pan; }
    void setPan(double pan);

    auto balance() const noexcept -> double { return m_balance; }
    void setBalance(double balance);

    bool mono() const noexcept { return m_mono; }

signals:

    void enabledChanged();
    void gainChanged();
    void levelLeftChanged();
    void levelRightChanged();
    void panChanged();
    void balanceChanged();
    void monoChanged();

private:
    double m_gain{1.};
    double m_levelLeft{};
    double m_levelRight{};
    double m_pan{};
    double m_balance{};
    bool m_enabled{true};
    bool m_mono{true};
};

} // namespace piejam::gui::model
