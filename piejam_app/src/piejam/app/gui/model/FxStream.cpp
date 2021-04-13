// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/FxStream.h>

#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/runtime/actions/request_streams_update.h>
#include <piejam/runtime/selectors.h>

namespace piejam::app::gui::model
{

struct FxStream::Impl
{
    FxStreamKeyId fxStreamKeyId;
};

FxStream::FxStream(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        FxStreamKeyId const& fxStreamKeyId)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(fxStreamKeyId))
{
}

FxStream::~FxStream() = default;

void
FxStream::onSubscribe()
{
    observe(runtime::selectors::make_audio_stream_selector(
                    m_impl->fxStreamKeyId.id),
            [this](runtime::audio_stream_buffer const& buf) {
                if (auto l = listener(); l && !buf->empty())
                    l->update(buf->frames());
            });
}

void
FxStream::requestUpdate()
{
    runtime::actions::request_streams_update action;
    action.streams.emplace(m_impl->fxStreamKeyId.id);
    dispatch(action);
}

} // namespace piejam::app::gui::model
