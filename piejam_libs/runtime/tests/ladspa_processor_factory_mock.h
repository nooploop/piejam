// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/sample_rate.h>
#include <piejam/ladspa/processor_factory.h>

#include <gmock/gmock.h>

namespace piejam::runtime::test
{

struct ladspa_processor_factory_mock : public ladspa::processor_factory
{
    MOCK_METHOD(
            std::unique_ptr<audio::engine::processor>,
            make_processor,
            (ladspa::instance_id const&, audio::sample_rate));
};

} // namespace piejam::runtime::test
