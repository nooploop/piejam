// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>

namespace piejam::thread
{

class semaphore
{
public:
    semaphore();
    semaphore(unsigned init_value);

    ~semaphore();

    semaphore(semaphore const&) = delete;
    semaphore(semaphore&&) noexcept = default;

    auto operator=(semaphore const&) -> semaphore& = delete;
    auto operator=(semaphore&& other) noexcept -> semaphore& = default;

    void acquire();

    void release();

private:
    struct impl;
    std::unique_ptr<impl> m_impl;
};

} // namespace piejam::thread
