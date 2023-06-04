// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023 Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStream.h>
#include <piejam/gui/model/BusType.h>
#include <piejam/gui/model/StereoChannel.h>

#include <piejam/algorithm/transform_to_vector.h>

#include <piejam/range/indices.h>
#include <piejam/range/zip.h>

namespace piejam::gui::model
{

struct SubStreamProcessorConfig
{
    BusType busType{BusType::Mono};
    bool active{false};
    StereoChannel channel{StereoChannel::Left};

    // substream info
    std::size_t startChannel{};
    std::size_t numChannels{};
};

template <class Result>
struct SubStreamProcessor
{
    virtual ~SubStreamProcessor() = default;

    virtual auto process(AudioStream const&) -> Result = 0;
    virtual void drop(std::size_t frames) = 0;
    virtual void clear() = 0;
};

template <class Args, class Result, class StreamProcessorFactory>
struct StreamProcessor
{
    explicit StreamProcessor(std::span<BusType const> substreamConfigs)
        : results{substreamConfigs.size()}
    {
        configs.reserve(substreamConfigs.size());
        processors.reserve(substreamConfigs.size());

        std::size_t startChannel{};
        for (BusType const busType : substreamConfigs)
        {
            auto& config = configs.emplace_back(SubStreamProcessorConfig{
                    .busType = busType,
                    .startChannel = startChannel,
                    .numChannels = num_channels(toBusType(busType))});

            processors.emplace_back(StreamProcessorFactory{}(config, args));

            startChannel += config.numChannels;
        }
    }

    void process(AudioStream const& stream)
    {
        for (std::size_t substreamIndex : range::indices(configs))
        {
            auto const& config = configs[substreamIndex];

            BOOST_ASSERT(substreamIndex < processors.size());
            BOOST_ASSERT(substreamIndex < results.size());
            results[substreamIndex] =
                    processors[substreamIndex]->process(stream.channels_subview(
                            config.startChannel,
                            config.numChannels));
        }
    }

    void drop(std::size_t const frames)
    {
        for (auto&& processor : processors)
        {
            processor->drop(frames);
        }
    }

    void clear()
    {
        for (auto&& processor : processors)
        {
            processor->clear();
        }
    }

    template <class T>
    void updateProperty(T& member, T value)
    {
        if (member != value)
        {
            member = std::move(value);

            for (auto&& [config, processor] : range::zip(configs, processors))
            {
                processor = StreamProcessorFactory{}(config, args);
            }
        }
    }

    void setActive(std::size_t const substreamIndex, bool const active)
    {
        BOOST_ASSERT(substreamIndex < configs.size());
        updateProperty(configs[substreamIndex].active, active);
    }

    void setStereoChannel(
            std::size_t const substreamIndex,
            StereoChannel const channel)
    {
        BOOST_ASSERT(substreamIndex < configs.size());
        updateProperty(configs[substreamIndex].channel, channel);
    }

    std::vector<Result> results;
    std::vector<SubStreamProcessorConfig> configs;
    std::vector<std::unique_ptr<SubStreamProcessor<Result>>> processors;

    Args args{};
};

} // namespace piejam::gui::model
