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
#include <QStringList>

#include <memory>
#include <string>
#include <vector>

namespace piejam::gui::model
{

class Info : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double audioLoad READ audioLoad NOTIFY audioLoadChanged FINAL)
    Q_PROPERTY(unsigned xruns READ xruns NOTIFY xrunsChanged FINAL)
    Q_PROPERTY(QStringList logData READ logData NOTIFY logDataChanged FINAL)

public:
    auto audioLoad() const noexcept -> double { return m_audioLoad; }
    void setAudioLoad(double);

    auto xruns() const noexcept -> unsigned { return m_xruns; }
    void setXRuns(unsigned xruns);

    auto logData() const noexcept -> QStringList { return m_logData; }
    void addLogMessage(QString const&);

    virtual Q_INVOKABLE void requestUpdate() = 0;

signals:

    void audioLoadChanged();
    void xrunsChanged();
    void logDataChanged();

private:
    double m_audioLoad{};
    unsigned m_xruns{};
    QStringList m_logData;
};

} // namespace piejam::gui::model
