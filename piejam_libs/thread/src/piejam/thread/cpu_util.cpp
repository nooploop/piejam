// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/thread/cpu_util.h>

#if defined(__x86_64__)
#include <pmmintrin.h> // for _MM_SET_DENORMALS_ZERO_MODE
#include <xmmintrin.h> // for _MM_SET_FLUSH_ZERO_MODE
#elif defined(__aarch64__) || defined(__arm__)
#include <arm_acle.h> // for __get_FPSCR / __set_FPSCR on ARM32

#include <cstdint>
#endif

namespace piejam::this_thread
{

void
enable_flush_to_zero() noexcept
{
#if defined(__x86_64__)
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#elif defined(__aarch64__)
    std::uint64_t fpcr;
    asm volatile("mrs %0, fpcr" : "=r"(fpcr));
    fpcr |= (1ull << 24);
    asm volatile("msr fpcr, %0" : : "r"(fpcr) : "memory");
#elif defined(__arm__)
    std::uint32_t fpscr;
    asm volatile("vmrs %0, fpscr" : "=r"(fpscr));
    fpscr |= (1u << 24);
    asm volatile("vmsr fpscr, %0" : : "r"(fpscr) : "memory");
#else
#warning "flush-to-zero not implemented on this platform"
#endif
}

void
cpu_spin_yield() noexcept
{
#if defined(__x86_64__)
    _mm_pause(); // SSE pause
#elif defined(__aarch64__)
    asm volatile("yield" ::: "memory");
#elif defined(__arm__)
#ifdef __ARM_ACLE
    __yield();
#else
    asm volatile("yield");
#endif
#else
#warning "cpu_spin_yield not implemented on this platform"
#endif
}

} // namespace piejam::this_thread
