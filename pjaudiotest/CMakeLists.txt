add_executable(pjaudiotest pjaudiotest.cpp)
target_link_libraries(pjaudiotest piejam_runtime spdlog::spdlog)
install(TARGETS pjaudiotest RUNTIME DESTINATION bin)
