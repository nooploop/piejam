// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
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

    M_PIEJAM_GUI_PROPERTY(QStringList, channels, setChannels)
    M_PIEJAM_GUI_READONLY_PROPERTY(QAbstractListModel*, deviceConfigs)

protected:
    AudioInputOutputSettings(
            runtime::store_dispatch,
            runtime::subscriber&,
            io_direction);

public:
    ~AudioInputOutputSettings() override;

    Q_INVOKABLE void addMonoDevice();
    Q_INVOKABLE void addStereoDevice();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
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
