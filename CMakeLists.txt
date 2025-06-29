cmake_minimum_required(VERSION 3.22)

# windows config vcpkg
if(WIN32)
    message("ENV{VCPKG_ROOT}: " $ENV{VCPKG_ROOT})
    # set(VCPKG_TARGET_TRIPLET "x64-windows-static")
    if(DEFINED ENV{VCPKG_ROOT})
        set(VCPKG_ROOT_COPY "$ENV{VCPKG_ROOT}")
        string(REPLACE "\\" "/" VCPKG_ROOT_FIXED "${VCPKG_ROOT_COPY}")
        set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT_FIXED}/scripts/buildsystems/vcpkg.cmake")
    else()
        message(FATAL_ERROR "NOT FIND ENVIRONMENT VARIABLE VCPKG_ROOT")
    endif()
    set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
    message("CMAKE_TOOLCHAIN_FILE: " ${CMAKE_TOOLCHAIN_FILE})
    message("VCPKG_TARGET_TRIPLET: " ${VCPKG_TARGET_TRIPLET})
endif(WIN32) 

project(DefendBillions)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
# set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

if(WIN32)
    add_custom_target(CopyCompileCommands COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" 
        "${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json" 
    )
endif()
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

file(GLOB_RECURSE DefendBillions_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
file(GLOB_RECURSE DefendBillions_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${DefendBillions_HEADERS} ${DefendBillions_SOURCES}
)

add_library(DefendBillions SHARED ${DefendBillions_HEADERS} ${DefendBillions_SOURCES})
# set_target_properties(DefendBillions PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
target_include_directories(DefendBillions PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# set_target_properties(DefendBillions PROPERTIES VS_WINRT_COMPONENT TRUE)
# set_target_properties(DefendBillions PROPERTIES
#     VS_DEBUGGER_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/Godot_v4.4-stable_win64.exe"
#     VS_DEBUGGER_COMMAND_ARGUMENTS "-e --path ${CMAKE_CURRENT_SOURCE_DIR}/gdp --verbose"
#     VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
# )

if(WIN32)
    set_target_properties(DefendBillions PROPERTIES PREFIX "lib")
endif()

foreach(CONFIGURATION_TYPE ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIGURATION_TYPE} CONFIG_TYPE_TOUPPER)
    set_target_properties(DefendBillions PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE_TOUPPER} "${CMAKE_CURRENT_SOURCE_DIR}/gdp/bin/")
    set_target_properties(DefendBillions PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE_TOUPPER} "${CMAKE_CURRENT_SOURCE_DIR}/gdp/bin/")
    set_target_properties(DefendBillions PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE_TOUPPER} "${CMAKE_CURRENT_SOURCE_DIR}/gdp/bin/")
    # MSVC Only, ignored on other platforms
    set_target_properties(DefendBillions PROPERTIES PDB_OUTPUT_DIRECTORY_${CONFIG_TYPE_TOUPPER}     "${CMAKE_CURRENT_SOURCE_DIR}/gdp/bin/")
    # set_target_properties(DefendBillions PROPERTIES ${CONFIG_TYPE_TOUPPER}_POSTFIX  ".${CONFIGURATION_TYPE}")
endforeach(CONFIGURATION_TYPE CMAKE_CONFIGURATION_TYPES)
set_target_properties(DefendBillions PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/gdp/bin/")
set_target_properties(DefendBillions PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/gdp/bin/")
set_target_properties(DefendBillions PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/gdp/bin/")
# MSVC Only, ignored on other platforms
set_target_properties(DefendBillions PROPERTIES PDB_OUTPUT_DIRECTORY    "${CMAKE_CURRENT_SOURCE_DIR}/gdp/bin/")
set_target_properties(DefendBillions PROPERTIES DEBUG_POSTFIX  "_debug")
# set_target_properties(DefendBillions PROPERTIES RELEASE_POSTFIX  "_r")

target_include_directories(DefendBillions PUBLIC "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")

include(FetchContent)

# gdextension
FetchContent_Declare(
        GDExtension
        GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
        GIT_TAG godot-4.4-stable
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/gdextension"
        BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/lib/gdextension"
)
FetchContent_MakeAvailable(GDExtension)
target_link_libraries(DefendBillions PUBLIC godot::cpp)

# function(print_all_variables)
# message(STATUS "CMake Variables:")
# get_cmake_property(_variableNames VARIABLES)
# list(SORT _variableNames)
# foreach(_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
# endfunction()

# print_all_variables()

# # pch
# include(GenerateExportHeader)
# generate_export_header(DefendBillions
#     EXPORT_MACRO_NAME NONHUMAN_API
#     EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/nonhuman_api.h
# )
# target_precompile_headers(DefendBillions PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/nonhuman_api.h)
