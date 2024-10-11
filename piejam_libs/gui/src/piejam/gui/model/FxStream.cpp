// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxStream.h>

#include <piejam/gui/model/AudioStreamListener.h>

#include <piejam/audio/multichannel_buffer.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

struct FxStream::Impl
{
    StreamId streamId;
};

FxStream::FxStream(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        StreamId const& streamId)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl{make_pimpl<Impl>(streamId)}
{
}

void
FxStream::onSubscribe()
{
    observe(runtime::selectors::make_audio_stream_selector(m_impl->streamId),
            [this](runtime::audio_stream_buffer const& buf) {
                if (!buf->empty())
                {
                    emit captured(buf->view());
                }
            });
}

} // namespace piejam::gui::model
