// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/io_direction.h>

#include <QStringList>

#include <memory>

namespace piejam::gui::model
{

class AudioInputOutputSettings : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QStringList channels READ channels NOTIFY channelsChanged FINAL)
    Q_PROPERTY(QAbstractListModel* busConfigs READ busConfigs CONSTANT FINAL)

protected:
    AudioInputOutputSettings(
            runtime::store_dispatch,
            runtime::subscriber&,
            io_direction);

public:
    ~AudioInputOutputSettings();

    auto channels() -> QStringList { return m_channels; }
    void setChannels(QStringList const& channels)
    {
        if (m_channels != channels)
        {
            m_channels = channels;
            emit channelsChanged();
        }
    }

    auto busConfigs() -> BusConfigsList*;

    Q_INVOKABLE void addMonoBus();
    Q_INVOKABLE void addStereoBus();

signals:
    void channelsChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QStringList m_channels;
};

class AudioInputSettings final : public AudioInputOutputSettings
{
public:
    AudioInputSettings(runtime::store_dispatch st, runtime::subscriber& subs)
        : AudioInputOutputSettings(st, subs, io_direction::input)
    {
    }
};

class AudioOutputSettings final : public AudioInputOutputSettings
{
public:
    AudioOutputSettings(runtime::store_dispatch st, runtime::subscriber& subs)
        : AudioInputOutputSettings(st, subs, io_direction::output)
    {
    }
};

} // namespace piejam::gui::model
