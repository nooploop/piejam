# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_subdirectory(algorithm)
add_subdirectory(system)
add_subdirectory(audio)
add_subdirectory(midi)
add_subdirectory(base)
add_subdirectory(functional)
add_subdirectory(log)
add_subdirectory(numeric)
add_subdirectory(range)
add_subdirectory(ladspa)
add_subdirectory(redux)
add_subdirectory(reselect)
add_subdirectory(thread)
add_subdirectory(runtime)
add_subdirectory(gui)
add_subdirectory(fx_modules)

set(PIEJAM_QML_IMPORT_PATHS ${PIEJAM_QML_IMPORT_PATHS} PARENT_SCOPE)
