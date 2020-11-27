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

#include <piejam/gui/model/SubscribableModel.h>

namespace piejam::gui::model
{

class MixerChannel : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(double volume READ volume NOTIFY volumeChanged FINAL)
    Q_PROPERTY(double levelLeft READ levelLeft NOTIFY levelLeftChanged FINAL)
    Q_PROPERTY(double levelRight READ levelRight NOTIFY levelRightChanged FINAL)
    Q_PROPERTY(double panBalance READ panBalance NOTIFY panBalanceChanged FINAL)
    Q_PROPERTY(bool mute READ mute NOTIFY muteChanged FINAL)
    Q_PROPERTY(bool solo READ solo NOTIFY soloChanged FINAL)

public:
    using SubscribableModel::SubscribableModel;

    auto name() const noexcept -> QString const& { return m_name; }
    void setName(QString const&);

    auto volume() const noexcept -> double { return m_volume; }
    void setVolume(double);
    Q_INVOKABLE virtual void changeVolume(double) = 0;

    auto levelLeft() const noexcept -> double { return m_levelLeft; }
    auto levelRight() const noexcept -> double { return m_levelRight; }
    void setLevel(double left, double right);

    auto panBalance() const noexcept -> double { return m_panBalance; }
    void setPanBalance(double);
    Q_INVOKABLE virtual void changePanBalance(double) = 0;

    auto mute() const noexcept -> bool { return m_mute; }
    void setMute(bool);
    Q_INVOKABLE virtual void changeMute(bool) = 0;

    auto solo() const noexcept -> bool { return m_solo; }
    void setSolo(bool);

    auto toQString() const -> QString { return m_name; }

signals:

    void nameChanged();
    void volumeChanged();
    void levelLeftChanged();
    void levelRightChanged();
    void panBalanceChanged();
    void muteChanged();
    void soloChanged();

private:
    QString m_name;
    double m_volume{1.};
    double m_levelLeft{};
    double m_levelRight{};
    double m_panBalance{};
    bool m_mute{};
    bool m_solo{};
};

} // namespace piejam::gui::model
