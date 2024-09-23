# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)

set(RESOURCE_FILES "resources/piejam_fx_modules_resources.qrc")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    find_package(Qt5QuickCompiler)
    qtquick_compiler_add_resources(RESOURCES ${RESOURCE_FILES})
else()
    qt5_add_resources(RESOURCES ${RESOURCE_FILES})
endif()

list(APPEND PIEJAM_QML_IMPORT_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(PIEJAM_QML_IMPORT_PATHS ${PIEJAM_QML_IMPORT_PATHS} PARENT_SCOPE)

add_library(piejam_fx_modules STATIC
    ${RESOURCES}

    include/piejam/fx_modules/init.h
    include/piejam/fx_modules/module_registration.h
    src/piejam/fx_modules/init.cpp
    src/piejam/fx_modules/module_registration.cpp

    include/piejam/fx_modules/filter/filter_component.h
    include/piejam/fx_modules/filter/filter_internal_id.h
    include/piejam/fx_modules/filter/filter_module.h
    include/piejam/fx_modules/filter/gui/FxFilter.h
    src/piejam/fx_modules/filter/filter_component.cpp
    src/piejam/fx_modules/filter/filter_internal_id.cpp
    src/piejam/fx_modules/filter/filter_module.cpp
    src/piejam/fx_modules/filter/gui/FxFilter.cpp

    include/piejam/fx_modules/scope/gui/FxScope.h
    include/piejam/fx_modules/scope/scope_component.h
    include/piejam/fx_modules/scope/scope_internal_id.h
    include/piejam/fx_modules/scope/scope_module.h
    src/piejam/fx_modules/scope/gui/FxScope.cpp
    src/piejam/fx_modules/scope/scope_component.cpp
    src/piejam/fx_modules/scope/scope_internal_id.cpp
    src/piejam/fx_modules/scope/scope_module.cpp

    include/piejam/fx_modules/spectrum/gui/FxSpectrum.h
    include/piejam/fx_modules/spectrum/spectrum_component.h
    include/piejam/fx_modules/spectrum/spectrum_internal_id.h
    include/piejam/fx_modules/spectrum/spectrum_module.h
    src/piejam/fx_modules/spectrum/gui/FxSpectrum.cpp
    src/piejam/fx_modules/spectrum/spectrum_component.cpp
    src/piejam/fx_modules/spectrum/spectrum_internal_id.cpp
    src/piejam/fx_modules/spectrum/spectrum_module.cpp

    include/piejam/fx_modules/dual_pan/gui/FxDualPan.h
    include/piejam/fx_modules/dual_pan/dual_pan_component.h
    include/piejam/fx_modules/dual_pan/dual_pan_internal_id.h
    include/piejam/fx_modules/dual_pan/dual_pan_module.h
    src/piejam/fx_modules/dual_pan/gui/FxDualPan.cpp
    src/piejam/fx_modules/dual_pan/dual_pan_component.cpp
    src/piejam/fx_modules/dual_pan/dual_pan_internal_id.cpp
    src/piejam/fx_modules/dual_pan/dual_pan_module.cpp

    include/piejam/fx_modules/tool/gui/FxTool.h
    include/piejam/fx_modules/tool/tool_component.h
    include/piejam/fx_modules/tool/tool_internal_id.h
    include/piejam/fx_modules/tool/tool_module.h
    src/piejam/fx_modules/tool/gui/FxTool.cpp
    src/piejam/fx_modules/tool/tool_component.cpp
    src/piejam/fx_modules/tool/tool_internal_id.cpp
    src/piejam/fx_modules/tool/tool_module.cpp

    include/piejam/fx_modules/tuner/tuner_internal_id.h
    src/piejam/fx_modules/tuner/tuner_internal_id.cpp
    include/piejam/fx_modules/tuner/gui/FxTuner.h
    include/piejam/fx_modules/tuner/tuner_component.h
    include/piejam/fx_modules/tuner/tuner_module.h
    src/piejam/fx_modules/tuner/tuner_module.cpp
    src/piejam/fx_modules/tuner/tuner_component.cpp
    src/piejam/fx_modules/tuner/gui/FxTuner.cpp
)

target_include_directories(piejam_fx_modules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(piejam_fx_modules PRIVATE -Wall -Wextra -Werror -pedantic-errors)

target_link_libraries(piejam_fx_modules
    piejam_runtime
    piejam_gui
)

unset(RESOURCE_FILES)
unset(RESOURCES)
