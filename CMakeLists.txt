cmake_minimum_required(VERSION 3.31)
project(ryujinIII_gui)

set(CMAKE_MAJOR_VERSION 1)
set(CMAKE_MINOR_VERSION 0)
set(CMAKE_PATCH_VERSION 0)
set(CMAKE_TWEAK_VERSION 0)
set(PROJECT_VERSION "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}.${CMAKE_TWEAK_VERSION}")

configure_file(inc/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/inc/version.h)
configure_file(README.md ${CMAKE_CURRENT_BINARY_DIR}/README.md COPYONLY)
configure_file(empty.svg empty.svg COPYONLY)

include(FetchContent)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM4 REQUIRED gtkmm-4.0)
FetchContent_Declare(
        ryujinIII_library
        GIT_REPOSITORY https://github.com/iscluis06/ryujin_III_software_control
        GIT_TAG master
)
FetchContent_Declare(
        CheckButtonMedia
        GIT_REPOSITORY https://github.com/iscluis06/gtkmm_checkbutton_media
        GIT_TAG master
)
FetchContent_Declare(
        GTKMMCustomEntry
        GIT_REPOSITORY https://github.com/iscluis06/gtkmm_custom_entry
        GIT_TAG master
)
FetchContent_MakeAvailable(ryujinIII_library CheckButtonMedia GTKMMCustomEntry)

set(CMAKE_CXX_STANDARD 17)

add_executable(ryujinIII_gui main.cpp
        src/window.cpp)

target_include_directories(ryujinIII_gui PUBLIC ${ryujiniii_library_SOURCE_DIR}/inc/ryujinIII_library ${CheckButtonMedia_SOURCE_DIR}/inc ${GTKMMCustomEntry_SOURCE_DIR}/inc ${GTKMM4_INCLUDE_DIRS} inc/)
target_link_libraries(ryujinIII_gui PUBLIC ryujinIII_library CheckbuttonMedia GTKMMCustomEntry ${GTKMM4_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin)
include(packaging.cmake)