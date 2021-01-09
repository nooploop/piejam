// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/SubscribableModel.h>

#include <QStringList>

#include <memory>
#include <string>
#include <vector>

namespace piejam::gui::model
{

class Info : public SubscribableModel
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
