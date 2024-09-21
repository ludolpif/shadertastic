# this file need to be use with add_subdirectory() from root CMakeLists.txt
find_package(ZLIB 1.2 REQUIRED)

add_executable(shadership)

target_sources(shadership PRIVATE shadership.cpp)

set_target_properties_plugin(shadership PROPERTIES FOLDER plugins/shadertastic)

# override some configuration from set_target_properties_plugin()
set_target_properties(shadership PROPERTIES VERSION 1)
target_link_libraries(shadership PRIVATE ZLIB::ZLIB)
