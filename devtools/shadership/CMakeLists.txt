add_executable(shadership)

target_sources(shadership PRIVATE shadership.cpp)
target_link_libraries(shadership PRIVATE
    plugin-support
    OBS::libobs
    libzip::zip
)
set_target_properties(shadership PROPERTIES FOLDER plugins/shadertastic)
install(TARGETS shadership RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
