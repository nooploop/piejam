// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/dsp/biquad_filter.h>
#include <piejam/audio/dsp/envelope_follower.h>

#include <boost/program_options.hpp>

#include <cmath>
#include <functional>
#include <iostream>
#include <numbers>
#include <optional>

enum class filter_type
{
    lp2,
    lp4,
    bp2,
    bp4,
    hp2,
    hp4,
    br
};

using filter_fn_t = std::function<float(float)>;

class sine_generator
{
public:
    sine_generator(float f, float sr)
        : m_cycles_per_sample(f / sr)
    {
    }

    auto operator()() noexcept -> float
    {
        return std::sin(
                2.f * std::numbers::pi_v<float> * (m_pos++) *
                m_cycles_per_sample);
    }

private:
    float m_cycles_per_sample;
    int m_pos{};
};

auto
make_filter(
        filter_type const ft,
        float const cutoff,
        float const res,
        float const sr) -> filter_fn_t
{
    float const inv_sr = 1.f / sr;
    switch (ft)
    {
        case filter_type::lp2:
            return [flt = piejam::audio::dsp::biquad<float>(
                            piejam::audio::dsp::biquad_filter::
                                    make_lp_coefficients(cutoff, res, inv_sr))](
                           float x) mutable -> float { return flt.process(x); };

        case filter_type::lp4:
            return [flt1 = piejam::audio::dsp::biquad<float>(
                            piejam::audio::dsp::biquad_filter::
                                    make_lp_coefficients(cutoff, res, inv_sr)),
                    flt2 = piejam::audio::dsp::biquad<float>(
                            piejam::audio::dsp::biquad_filter::
                                    make_lp_coefficients(cutoff, res, inv_sr))](
                           float x) mutable -> float {
                return flt2.process(flt1.process(x));
            };

        case filter_type::bp2:
            return [flt = piejam::audio::dsp::biquad<float>(
                            piejam::audio::dsp::biquad_filter::
                                    make_bp_coefficients(cutoff, res, inv_sr))](
                           float x) mutable -> float { return flt.process(x); };

        case filter_type::bp4:
            return [flt1 = piejam::audio::dsp::biquad<float>(
                            piejam::audio::dsp::biquad_filter::
                                    make_bp_coefficients(cutoff, res, inv_sr)),
                    flt2 = piejam::audio::dsp::biquad<float>(
                            piejam::audio::dsp::biquad_filter::
                                    make_bp_coefficients(cutoff, res, inv_sr))](
                           float x) mutable -> float {
                return flt2.process(flt1.process(x));
            };

        case filter_type::hp2:
            return [flt = piejam::audio::dsp::biquad<float>(
                            piejam::audio::dsp::biquad_filter::
                                    make_hp_coefficients(cutoff, res, inv_sr))](
                           float x) mutable -> float { return flt.process(x); };

        case filter_type::hp4:
            return [flt1 = piejam::audio::dsp::biquad<float>(
                            piejam::audio::dsp::biquad_filter::
                                    make_hp_coefficients(cutoff, res, inv_sr)),
                    flt2 = piejam::audio::dsp::biquad<float>(
                            piejam::audio::dsp::biquad_filter::
                                    make_hp_coefficients(cutoff, res, inv_sr))](
                           float x) mutable -> float {
                return flt2.process(flt1.process(x));
            };

        case filter_type::br:
            return [flt1 = piejam::audio::dsp::biquad<float>(
                            piejam::audio::dsp::biquad_filter::
                                    make_br_coefficients(cutoff, res, inv_sr)),
                    flt2 = piejam::audio::dsp::biquad<float>(
                            piejam::audio::dsp::biquad_filter::
                                    make_br_coefficients(cutoff, res, inv_sr))](
                           float x) mutable -> float {
                return flt2.process(flt1.process(x));
            };

        default:
            return [](float x) { return x; };
    }
}

auto
main(int argc, char* argv[]) -> int
{
    namespace po = boost::program_options;

    static auto const s_help_str = "help";
    static auto const s_type_str = "type";
    static auto const s_cutoff_str = "cutoff";
    static auto const s_res_str = "res";
    static auto const s_sample_rate_str = "sample_rate";

    po::options_description desc("Options");
    desc.add_options()(s_help_str, "produce help message")(
            s_type_str,
            po::value<std::string>(),
            "filter type:\n"
            "lp2 - Biquad low pass 2 pole\n"
            "lp4 - Biquad low pass 4 pole\n"
            "bp2 - Biquad band pass 2 pole\n"
            "bp4 - Biquad band pass 4 pole\n"
            "hp2 - Biquad high pass 2 pole\n"
            "hp4 - Biquad high pass 4 pole\n"
            "br - Biquad band reject\n")(
            s_cutoff_str,
            po::value<float>(),
            "cutoff frequency (20-20000)")(
            s_res_str,
            po::value<float>(),
            "resonance (0-1)")(
            s_sample_rate_str,
            po::value<unsigned>(),
            "sample rate, e.g. 44100, 48000");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count(s_help_str))
    {
        std::cout << desc << std::endl;
        return 1;
    }

    unsigned sample_rate{};
    if (vm.count(s_sample_rate_str))
    {
        sample_rate = vm[s_sample_rate_str].as<unsigned>();
    }
    else
    {
        std::cerr << "Sample rate not set!" << std::endl;
        return -1;
    }

    float cutoff{};
    if (vm.count(s_cutoff_str))
    {
        cutoff = vm[s_cutoff_str].as<float>();
    }
    else
    {
        std::cerr << "Cutoff frequency not set!" << std::endl;
        return -1;
    }

    float resonance{};
    if (vm.count(s_res_str))
    {
        resonance = vm[s_res_str].as<float>();
    }
    else
    {
        std::cerr << "Resonance not set!" << std::endl;
        return -1;
    }

    std::optional<filter_type> ft;
    if (vm.count(s_type_str))
    {
        std::string const ft_value = vm[s_type_str].as<std::string>();
        if (ft_value == "lp2")
            ft = filter_type::lp2;
        else if (ft_value == "lp4")
            ft = filter_type::lp4;
        else if (ft_value == "bp2")
            ft = filter_type::bp2;
        else if (ft_value == "bp4")
            ft = filter_type::bp4;
        else if (ft_value == "hp2")
            ft = filter_type::hp2;
        else if (ft_value == "hp4")
            ft = filter_type::hp4;
        else if (ft_value == "br")
            ft = filter_type::br;
        else
        {
            std::cerr << "Unknown filter type!" << std::endl;
            return -1;
        }
    }
    else
    {
        std::cerr << "Filter type not set!" << std::endl;
        return -1;
    }

    for (unsigned freq = 20; freq <= 20000; freq += 1)
    {
        auto flt = make_filter(*ft, cutoff, resonance, sample_rate);

        // run for 32 cycles
        auto num_samples =
                static_cast<std::size_t>((32.f * sample_rate) / cutoff);

        piejam::audio::dsp::peak_envelope_follower<float> follower(num_samples);

        float peak{};
        sine_generator gen(freq, sample_rate);
        for (unsigned n = 0; n < num_samples; ++n)
        {
            peak = follower(flt(gen()));
        }

        std::cout << freq << " " << peak << std::endl;
    }

    return 0;
}
