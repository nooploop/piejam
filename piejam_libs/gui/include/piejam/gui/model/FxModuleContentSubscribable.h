// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/gui/model/FxParameterId.h>
#include <piejam/gui/model/FxStreamKeyId.h>
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
    template <class Parameter, class Parameters>
    void makeParameter(
            std::size_t key,
            std::unique_ptr<Parameter>& param,
            Parameters const& parameters)
    {
        param = std::make_unique<Parameter>(
                dispatch(),
                this->state_change_subscriber(),
                parameters.at(key));
        connectSubscribableChild(*param);
    }

    template <class Stream, class Streams>
    void makeStream(
            std::size_t key,
            std::unique_ptr<Stream>& stream,
            Streams const& streams)
    {
        stream = std::make_unique<Stream>(
                dispatch(),
                this->state_change_subscriber(),
                FxStreamKeyId{.key = key, .id = streams.at(key)});
        connectSubscribableChild(*stream);
    }
};

} // namespace piejam::gui::model
