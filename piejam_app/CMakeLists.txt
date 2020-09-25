set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core)

set(RESOUCE_FILES "resources/piejam_app_resources.qrc")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    find_package(Qt5QuickCompiler)
    qtquick_compiler_add_resources(RESOURCES ${RESOUCE_FILES})
else()
    qt5_add_resources(RESOURCES ${RESOUCE_FILES})
endif()

add_executable(piejam_app
    ${RESOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/config_access.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/config_access.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/AudioInputOutputSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/AudioInputOutputSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/AudioDeviceSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/AudioDeviceSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/BusConfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/BusConfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/Info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/Info.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/Mixer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/Mixer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/Subscribable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/store.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/subscriber.h
)
target_include_directories(piejam_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(piejam_app PRIVATE piejam_runtime piejam_redux piejam_algorithm piejam_log piejam_gui)
target_compile_options(piejam_app PRIVATE -Wall -Wextra -Werror -Wno-error=deprecated-declarations -pedantic-errors)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(piejam_app PRIVATE QT_QML_DEBUG=1)
endif()
install(TARGETS piejam_app RUNTIME DESTINATION bin RENAME piejam)
