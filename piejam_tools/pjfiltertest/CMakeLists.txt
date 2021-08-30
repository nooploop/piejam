# SPDX-FileCopyrightText: 2021 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_executable(pjfiltertest pjfiltertest.cpp)
target_link_libraries(pjfiltertest piejam_runtime spdlog::spdlog Boost::program_options)
install(TARGETS pjfiltertest RUNTIME DESTINATION bin)
