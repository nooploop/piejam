// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
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
    explicit dft(std::size_t size);
    ~dft();

    [[nodiscard]] auto size() const noexcept -> std::size_t;
    [[nodiscard]] auto input_buffer() const noexcept -> std::span<float>;
    [[nodiscard]] auto output_size() const noexcept -> std::size_t;

    auto process() -> std::span<std::complex<float> const>;

private:
    struct impl;
    std::unique_ptr<impl> m_impl;
};

} // namespace piejam::numeric
