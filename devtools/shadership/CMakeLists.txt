find_package(ZLIB 1.2 REQUIRED)

add_executable(shadership)

target_sources(shadership PRIVATE shadership.cpp)

set_target_properties(shadership PROPERTIES FOLDER plugins/shadertastic)
install(TARGETS shadership RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

target_link_libraries(shadership PRIVATE
    plugin-support
    OBS::libobs
    ZLIB::ZLIB
)
