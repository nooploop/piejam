// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/ScopeLines.h>
#include <piejam/gui/model/fwd.h>

#include <QObject>

#include <memory>

namespace piejam::gui::model
{

class FxScope : public QObject
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::AudioStreamListener* streamListener READ
                       streamListener CONSTANT FINAL)
    Q_PROPERTY(int samplesPerPoint READ samplesPerPoint WRITE setSamplesPerPoint
                       NOTIFY samplesPerPointChanged FINAL)
    Q_PROPERTY(int viewSize READ viewSize WRITE setViewSize NOTIFY
                       viewSizeChanged FINAL)
    Q_PROPERTY(piejam::gui::model::ScopeLines* leftLines READ leftLines NOTIFY
                       leftLinesChanged FINAL)
    Q_PROPERTY(piejam::gui::model::ScopeLines* rightLines READ rightLines NOTIFY
                       rightLinesChanged FINAL)

public:
    FxScope(QObject* parent = nullptr);
    ~FxScope();

    auto streamListener() noexcept -> AudioStreamListener*;

    auto samplesPerPoint() const noexcept -> int;
    void setSamplesPerPoint(int);

    auto viewSize() const noexcept -> int;
    void setViewSize(int);

    auto leftLines() noexcept -> ScopeLines*;
    auto rightLines() noexcept -> ScopeLines*;

    Q_INVOKABLE void clear();

signals:
    void samplesPerPointChanged();
    void viewSizeChanged();
    void leftLinesChanged();
    void rightLinesChanged();
    void syncedChanged();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
