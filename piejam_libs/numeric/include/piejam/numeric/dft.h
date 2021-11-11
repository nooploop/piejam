// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <complex>
#include <memory>
#include <span>

namespace piejam::numeric
{

class dft
{
public:
    dft(std::size_t size);
    ~dft();

    auto input_size() const noexcept -> std::size_t;
    auto output_size() const noexcept -> std::size_t;

    auto process(std::span<float const>)
            -> std::span<std::complex<float> const>;

private:
    struct impl;
    std::unique_ptr<impl> m_impl;
};

} // namespace piejam::numeric
