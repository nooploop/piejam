// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/parameter_processor_factory.h>

#include <piejam/audio/engine/processor_test_environment.h>

#include <gtest/gtest.h>

#include <boost/hof/match.hpp>

namespace piejam::runtime::processors::test
{

struct int_param_fake
{
    using value_type = int;
};

struct float_param_fake
{
    using value_type = float;
};

using factory_t = parameter_processor_factory<int_param_fake, float_param_fake>;

TEST(parameter_processor_factory, find_non_existent)
{
    factory_t sut;
    auto id = parameter::id_t<int_param_fake>::generate();
    auto found_proc = sut.find_processor(id);

    EXPECT_EQ(nullptr, found_proc.get());
}

TEST(parameter_processor_factory, make_and_find)
{
    factory_t sut;
    auto id = parameter::id_t<int_param_fake>::generate();
    auto proc = sut.make_processor(id);
    auto found_proc = sut.find_processor(id);

    EXPECT_NE(nullptr, proc.get());
    EXPECT_EQ(proc.get(), found_proc.get());
}

TEST(parameter_processor_factory, make_and_find_expired)
{
    factory_t sut;
    auto id = parameter::id_t<int_param_fake>::generate();
    sut.make_processor(id);
    auto found_proc = sut.find_processor(id);

    EXPECT_EQ(nullptr, found_proc.get());
}

TEST(parameter_processor_factory, make_and_replace)
{
    factory_t sut;
    auto id = parameter::id_t<int_param_fake>::generate();
    auto proc1 = sut.make_processor(id);
    auto proc2 = sut.make_processor(id);
    auto found_proc = sut.find_processor(id);

    EXPECT_EQ(proc2.get(), found_proc.get());
}

TEST(parameter_processor_factory,
     initialize_will_send_an_event_to_out_on_process)
{
    factory_t sut;
    auto id = parameter::id_t<int_param_fake>::generate();
    auto proc = sut.make_processor(id);

    int value{5};
    sut.initialize(boost::hof::match(
            [&](parameter::id_t<int_param_fake> param_id) -> int const* {
                return param_id == id ? &value : nullptr;
            },
            [](parameter::id_t<float_param_fake>) -> float const* {
                return nullptr;
            }));

    audio::engine::processor_test_environment test_env(*proc, 16);

    proc->process(test_env.ctx);

    auto const& ev_out = test_env.event_outputs.get<int>(0);
    ASSERT_EQ(1u, ev_out.size());
    EXPECT_EQ(0u, ev_out.begin()->offset());
    EXPECT_EQ(value, ev_out.begin()->value());
}

TEST(parameter_processor_factory,
     initialize_without_a_value_will_not_send_anything)
{
    factory_t sut;
    auto id = parameter::id_t<int_param_fake>::generate();
    auto proc = sut.make_processor(id);

    sut.initialize(
            []<class P>(parameter::id_t<P>)
                    -> parameter::value_type_t<P> const* { return nullptr; });

    audio::engine::processor_test_environment test_env(*proc, 16);

    proc->process(test_env.ctx);

    auto const& ev_out = test_env.event_outputs.get<int>(0);
    EXPECT_TRUE(ev_out.empty());
}

TEST(parameter_processor_factory, set_will_send_an_event_to_out_on_process)
{
    factory_t sut;
    auto id = parameter::id_t<int_param_fake>::generate();
    auto proc = sut.make_processor(id);

    sut.set(id, 5);

    audio::engine::processor_test_environment test_env(*proc, 16);

    proc->process(test_env.ctx);

    auto const& ev_out = test_env.event_outputs.get<int>(0);
    ASSERT_EQ(1u, ev_out.size());
    EXPECT_EQ(0u, ev_out.begin()->offset());
    EXPECT_EQ(5, ev_out.begin()->value());
}

TEST(parameter_processor_factory, set_to_non_existent_processor)
{
    factory_t sut;
    auto id = parameter::id_t<int_param_fake>::generate();
    auto proc = sut.make_processor(id);

    sut.set(parameter::id_t<int_param_fake>::generate(), 5);

    audio::engine::processor_test_environment test_env(*proc, 16);

    proc->process(test_env.ctx);

    // verify that the processor is not sending anything
    auto const& ev_out = test_env.event_outputs.get<int>(0);
    EXPECT_TRUE(ev_out.empty());
}

TEST(parameter_processor_factory, consume_incoming_event)
{
    factory_t sut;
    auto id = parameter::id_t<int_param_fake>::generate();
    auto proc = sut.make_processor(id);

    audio::engine::processor_test_environment test_env(*proc, 16);
    test_env.insert_input_event<int>(0, 3, 9);

    proc->process(test_env.ctx);

    bool consume_called{};
    sut.consume(id, [&](int v) {
        consume_called = true;
        EXPECT_EQ(9, v);
    });
    EXPECT_TRUE(consume_called);
}

TEST(parameter_processor_factory,
     consume_functor_is_not_called_for_non_existing_processor)
{
    factory_t sut;

    sut.consume(parameter::id_t<int_param_fake>::generate(), [](int) {
        FAIL();
    });
}

TEST(parameter_processor_factory, make_parameter_processor)
{
    factory_t sut;
    auto id = parameter::id_t<int_param_fake>::generate();
    using param_id_t = std::variant<
            parameter::id_t<int_param_fake>,
            parameter::id_t<float_param_fake>>;
    auto proc = make_parameter_processor(sut, param_id_t(id));
    auto found_proc = sut.find_processor(id);

    EXPECT_EQ(proc.get(), found_proc.get());
}

TEST(parameter_processor_factory, clear_expired)
{
    factory_t sut;
    auto id = parameter::id_t<int_param_fake>::generate();
    sut.make_processor(id);

    EXPECT_TRUE(sut.has_expired());
    sut.clear_expired();
    EXPECT_FALSE(sut.has_expired());
}

} // namespace piejam::runtime::processors::test
