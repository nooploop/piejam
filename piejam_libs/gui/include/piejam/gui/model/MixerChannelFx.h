// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/MixerChannel.h>
#include <piejam/gui/model/StringList.h>

#include <piejam/audio/types.h>
#include <piejam/runtime/mixer_fwd.h>

#include <memory>

namespace piejam::gui::model
{

class MixerChannelFx final : public MixerChannel
{
    Q_OBJECT

    Q_PROPERTY(bool focused READ focused NOTIFY focusedChanged FINAL)
    Q_PROPERTY(QAbstractListModel* fxChain READ fxChain CONSTANT)
    Q_PROPERTY(bool canMoveUpFxModule READ canMoveUpFxModule NOTIFY
                       canMoveUpFxModuleChanged FINAL)
    Q_PROPERTY(bool canMoveDownFxModule READ canMoveDownFxModule NOTIFY
                       canMoveDownFxModuleChanged FINAL)

public:
    MixerChannelFx(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);
    ~MixerChannelFx() override;

    auto focused() const noexcept -> bool
    {
        return m_focused;
    }

    auto canMoveUpFxModule() const noexcept -> bool
    {
        return m_canMoveUpFxModule;
    }

    auto canMoveDownFxModule() const noexcept -> bool
    {
        return m_canMoveDownFxModule;
    }

    auto fxChain() noexcept -> FxChainModulesList*;

    Q_INVOKABLE void appendFxModule();
    Q_INVOKABLE void moveUpFxModule();
    Q_INVOKABLE void moveDownFxModule();

signals:
    void focusedChanged();
    void canMoveUpFxModuleChanged();
    void canMoveDownFxModuleChanged();

private:
    void setFocused(bool const x)
    {
        if (m_focused != x)
        {
            m_focused = x;
            emit focusedChanged();
        }
    }

    void setCanMoveUp(bool const x)
    {
        if (m_canMoveUpFxModule != x)
        {
            m_canMoveUpFxModule = x;
            emit canMoveUpFxModuleChanged();
        }
    }

    void setCanMoveDown(bool const x)
    {
        if (m_canMoveDownFxModule != x)
        {
            m_canMoveDownFxModule = x;
            emit canMoveDownFxModuleChanged();
        }
    }

    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    bool m_focused{};
    bool m_canMoveUpFxModule{};
    bool m_canMoveDownFxModule{};
};

} // namespace piejam::gui::model
