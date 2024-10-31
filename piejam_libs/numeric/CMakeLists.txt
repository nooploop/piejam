# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_numeric STATIC
    include/piejam/numeric/bit.h
    include/piejam/numeric/concepts.h
    include/piejam/numeric/dft.h
    include/piejam/numeric/endian.h
    include/piejam/numeric/fwd.h
    include/piejam/numeric/intops.h
    include/piejam/numeric/intx.h
    include/piejam/numeric/mipp.h
    include/piejam/numeric/mipp_iterator.h
    include/piejam/numeric/pow_n.h
    include/piejam/numeric/rolling_mean.h
    include/piejam/numeric/type_traits.h
    include/piejam/numeric/window.h
    src/piejam/numeric/dft.cpp
)

target_include_directories(piejam_numeric PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(piejam_numeric PRIVATE -Wall -Wextra -Werror -pedantic-errors)
target_link_libraries(piejam_numeric PUBLIC MIPP PRIVATE FFTW::Float)

add_subdirectory(tests)
