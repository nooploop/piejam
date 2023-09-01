// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/gui/model/ParameterId.h>
#include <piejam/gui/model/StreamId.h>
#include <piejam/gui/model/Subscribable.h>

#include <memory>

namespace piejam::gui::model
{

class FxModuleContentSubscribable : public Subscribable<FxModuleContent>
{
    using Base = Subscribable<FxModuleContent>;

public:
    using Base::Base;

protected:
    template <class ParameterT, class Parameters>
    void makeParameter(
            std::size_t key,
            std::unique_ptr<ParameterT>& param,
            Parameters const& parameters)
    {
        param = std::make_unique<ParameterT>(
                dispatch(),
                this->state_change_subscriber(),
                parameters.at(key));
        connectSubscribableChild(*param);
    }

    template <class StreamT, class Streams>
    void makeStream(
            std::size_t key,
            std::unique_ptr<StreamT>& stream,
            Streams const& streams)
    {
        stream = std::make_unique<StreamT>(
                dispatch(),
                this->state_change_subscriber(),
                streams.at(key));
        connectSubscribableChild(*stream);
    }
};

} // namespace piejam::gui::model
