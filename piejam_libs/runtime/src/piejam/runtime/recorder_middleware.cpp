// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/recorder_middleware.h>

#include <piejam/runtime/actions/audio_engine_sync.h>
#include <piejam/runtime/actions/recorder_action.h>
#include <piejam/runtime/actions/recording.h>
#include <piejam/runtime/middleware_functors.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/update_state_action.h>

#include <piejam/audio/dsp/mipp_iterator.h>
#include <piejam/audio/multichannel_buffer.h>
#include <piejam/system/file_utils.h>

#include <sndfile.hh>

#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>

#include <ranges>

namespace piejam::runtime
{

struct recorder_middleware::impl
{
    using open_streams_t =
            boost::container::flat_map<audio_stream_id, SndfileHandle>;

    std::filesystem::path recordings_dir;
    open_streams_t open_streams{};
};

recorder_middleware::recorder_middleware(std::filesystem::path recordings_dir)
    : m_impl(make_pimpl<impl>(std::move(recordings_dir)))
{
}

void
recorder_middleware::operator()(
        middleware_functors const& mw_fs,
        action const& act)
{
    if (auto const* a = dynamic_cast<actions::recorder_action const*>(&act))
    {
        auto v = ui::make_action_visitor<actions::recorder_action_visitor>(
                [this, &mw_fs](auto const& a) {
                    process_recorder_action(mw_fs, a);
                });
        a->visit(v);
    }
    else
    {
        mw_fs.next(act);
    }
}

template <class Action>
void
recorder_middleware::process_recorder_action(
        middleware_functors const&,
        Action const&)
{
    BOOST_ASSERT_MSG(false, "unhandled action");
}

template <>
void
recorder_middleware::process_recorder_action(
        middleware_functors const& mw_fs,
        actions::start_recording const&)
{
    auto const& st = mw_fs.get_state();

    BOOST_ASSERT(!st.recording);
    BOOST_ASSERT(st.recorder_streams->empty());

    auto take_dir = m_impl->recordings_dir /
                    fmt::format("session_{:06}", st.rec_session) /
                    fmt::format("take_{:04}", st.rec_take);

    std::error_code ec;
    if (!std::filesystem::create_directories(take_dir, ec))
    {
        auto const message = ec.message();
        spdlog::error("Could not create recordings directory: {}", message);
        return;
    }

    recorder_streams_t recorder_streams;
    auto streams = st.streams;
    impl::open_streams_t open_streams;

    for (auto const& [mixer_channel_id, mixer_channel] :
         st.mixer_state.channels)
    {
        if (!st.params[mixer_channel.record].value.get())
        {
            continue;
        }

        auto filename = system::make_unique_filename(
                take_dir,
                *mixer_channel.name,
                "wav");

        auto const format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;
        std::size_t const num_channels =
                audio::num_channels(mixer_channel.bus_type);
        if (SndfileHandle::formatCheck(
                    format,
                    static_cast<int>(num_channels),
                    st.sample_rate.as_int()))
        {
            SndfileHandle sndfile(
                    filename.string(),
                    SFM_WRITE,
                    format,
                    static_cast<int>(num_channels),
                    st.sample_rate.as_int());

            if (sndfile)
            {
                auto stream_id = streams.emplace(std::in_place, num_channels);
                recorder_streams.emplace(mixer_channel_id, stream_id);
                open_streams.emplace(stream_id, std::move(sndfile));
            }
            else
            {
                auto const* const message = sndfile.strError();
                spdlog::error(
                        "Could not create file for recording: {}",
                        message);
            }
        }
        else
        {
            spdlog::error(
                    "Could not create file for recording: Invalid file "
                    "format.");
        }
    }

    if (open_streams.empty())
    {
        return;
    }

    m_impl->open_streams = std::move(open_streams);

    mw_fs.next(update_state_action{
            [streams = std::move(streams),
             recorder_streams = std::move(recorder_streams)](state& st) {
                st.recording = true;
                st.recorder_streams = std::move(recorder_streams);
                st.streams = std::move(streams);
            }});
}

template <>
void
recorder_middleware::process_recorder_action(
        middleware_functors const& mw_fs,
        actions::stop_recording const&)
{
    BOOST_ASSERT(mw_fs.get_state().recording);

    m_impl->open_streams.clear();

    mw_fs.next(update_state_action{[](state& new_st) {
        new_st.recording = false;
        ++new_st.rec_take;

        new_st.streams.erase(*new_st.recorder_streams | std::views::values);

        new_st.recorder_streams = {};
    }});
}

template <>
void
recorder_middleware::process_recorder_action(
        middleware_functors const& mw_fs,
        actions::audio_engine_sync_update const& a)
{
    for (auto const& [stream_id, buffer] : a.streams)
    {
        if (auto it = m_impl->open_streams.find(stream_id);
            it != m_impl->open_streams.end())
        {
            auto const num_frames = buffer->num_frames();
            BOOST_ASSERT(
                    buffer->layout() ==
                    audio::multichannel_layout::non_interleaved);
            BOOST_ASSERT(
                    buffer->num_channels() == 1 || buffer->num_channels() == 2);

            auto write_data = buffer->samples();

            audio::multichannel_buffer<float>::vector interleaved;
            if (buffer->num_channels() == 2)
            {
                auto stereo_view =
                        buffer->view()
                                .cast<audio::multichannel_layout_non_interleaved,
                                      2>();
                interleaved.resize(stereo_view.samples().size());

                std::ranges::transform(
                        audio::dsp::mipp_range(stereo_view.channels()[0]),
                        audio::dsp::mipp_range(stereo_view.channels()[1]),
                        audio::dsp::make_mipp_iterator_x2(interleaved.data()),
                        [](auto reg_l, auto reg_r) {
                            return mipp::interleave(reg_l, reg_r);
                        });

                write_data = interleaved;
            }

            auto const written =
                    it->second.writef(write_data.data(), num_frames);
            if (static_cast<std::size_t>(written) < num_frames)
            {
                auto const frames_not_written = num_frames - written;
                auto const* const message = it->second.strError();
                spdlog::warn(
                        "Could not write {} frames: {}",
                        frames_not_written,
                        message);
            }
        }
    }

    mw_fs.next(a);
}

} // namespace piejam::runtime
