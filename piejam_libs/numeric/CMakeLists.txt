# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_numeric STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/bitops.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/dft.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/endian.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/intops.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/intx.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/limits.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/rolling_mean.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/type_traits.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/window.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/numeric/dft.cpp
)

target_include_directories(piejam_numeric PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(piejam_numeric PRIVATE -Wall -Wextra -Werror -pedantic-errors)
target_link_libraries(piejam_numeric PRIVATE FFTW::Float)

add_subdirectory(tests)
