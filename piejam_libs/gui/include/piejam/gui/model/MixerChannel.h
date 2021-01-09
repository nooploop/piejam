// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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

    Q_INVOKABLE virtual void focusFxChain() = 0;

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
