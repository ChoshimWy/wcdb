# version 1.0.7
include(CMakeParseArguments)
include(CheckFunctionExists)

# read WCONAN_PACKAGE_NAME from wconan_package.yaml
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../WeChat/wconan_package.yaml")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/../WeChat/wconan_package.yaml" TEMP_YAML)
    string(REGEX MATCH "[ \t]*name[ \t]*:[ \t]*([0-9a-zA-Z\\.\\_\\-]*)" _ ${TEMP_YAML})
    set(WCONAN_PACKAGE_NAME ${CMAKE_MATCH_1})
    message(STATUS "WCONAN_PACKAGE_NAME: ${WCONAN_PACKAGE_NAME}")
else ()
    message(FATAL "wconan_package.yaml NOT FOUND!")
endif ()

if (NOT WCONAN_LIB_NAME)
    string(REPLACE "-" "_" WCONAN_LIB_NAME ${WCONAN_PACKAGE_NAME})
endif ()
if (NOT WCONAN_LIB_VERSION)
    set(WCONAN_LIB_VERSION "0.0.0")
endif ()
if (APPLE)
    string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+).*" _ ${WCONAN_LIB_VERSION})
    if (CMAKE_MATCH_1)
        set(WCONAN_APPLE_LIB_VERSION "${CMAKE_MATCH_1}")
    else ()
        set(WCONAN_APPLE_LIB_VERSION "0.0.0")
    endif ()
endif ()

message(STATUS "wconan.cmake - WCONAN_PACKAGE_NAME: [${WCONAN_PACKAGE_NAME}], WCONAN_LIB_NAME: [${WCONAN_LIB_NAME}], WCONAN_LIB_VERSION: [${WCONAN_LIB_VERSION}] , WCONAN_APPLE_LIB_VERSION: [${WCONAN_APPLE_LIB_VERSION}]")

# auto generate lib_version.c
# when build with cmake, lib_version.c should contain a function like this:
#     extern "C" const char* get_owl_version();
set(SRC_LIB_VERSION ${CMAKE_CURRENT_SOURCE_DIR}/../WeChat/wconan_build/cmake/lib_version.c)
execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_ID
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "GIT_BRANCH ${GIT_BRANCH}, GIT_COMMIT_ID: ${GIT_COMMIT_ID}")
string(TOLOWER ${WCONAN_LIB_NAME} WCONAN_LIB_NAME_LOWER)
file(WRITE "${SRC_LIB_VERSION}" "const char* get_${WCONAN_LIB_NAME_LOWER}_version() { return \"${WCONAN_LIB_NAME} ${WCONAN_LIB_VERSION} ${GIT_BRANCH} ${GIT_COMMIT_ID}(\" __DATE__ \" \" __TIME__ \")\"; }")
set(DEFAULT_GIT_IGNORE ${CMAKE_CURRENT_SOURCE_DIR}/../WeChat/wconan_build/cmake/.gitignore)
if (NOT EXISTS "${DEFAULT_GIT_IGNORE}")
    file(WRITE "${DEFAULT_GIT_IGNORE}" "*")
endif ()
# platform define, available platforms:
# ANDROID, APPLE(MACOS/IOS/...), WIN32, LINUX
# for LINUX: use CMAKE_CROSSCOMPILING to distinguish normal Linux(Ubuntu, Centos, etc) and embedding Linux
# for ANDROID: use ANDROID_ABI to distinguish different arch("arm64-v8a", "armeabi-v7a", etc)
# for WIN32: use CMAKE_SIZEOF_VOID_P to distinguish different arch(X86_64, x86)
if (APPLE)
    message(STATUS "WCONAN_PLATFORM: Apple ${CMAKE_SYSTEM_NAME}")
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(MACOS TRUE)
        set(WCONAN_PLATFORM "macos")
        if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm64")
            set(WCONAN_LOCAL_PLATFORM "macos-all")
        else ()
            set(WCONAN_LOCAL_PLATFORM "macos")
        endif()
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "watchOS")
        set(WATCHOS TRUE)
        set(WCONAN_PLATFORM "watchos")
        set(WCONAN_LOCAL_PLATFORM "watchos")
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "tvOS")
        set(TVOS TRUE)
        set(WCONAN_PLATFORM "tvos")
        set(WCONAN_LOCAL_PLATFORM "tvos")
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
        set(IOS TRUE)
        set(WCONAN_PLATFORM "ios")
        set(WCONAN_LOCAL_PLATFORM "ios")
    else ()
        message(FATAL_ERROR "CMAKE_SYSTEM_NAME ${CMAKE_SYSTEM_NAME} is not supported")
    endif ()
elseif (ANDROID)
    message(STATUS "WCONAN_PLATFORM: ${ANDROID_PLATFORM} ${ANDROID_ABI}")
    set(WCONAN_PLATFORM "android")
    set(ANDROID_ARCH_LIST armeabi-v7a arm64-v8a x86 x86_64)
    if(${ANDROID_ABI} IN_LIST ANDROID_ARCH_LIST)
        set(WCONAN_LOCAL_PLATFORM "android-${ANDROID_ABI}")
    else()
        message(FATAL_ERROR "unsupported ANDROID_ABI ${ANDROID_ABI}")
    endif()
elseif (WIN32)
    set(WCONAN_PLATFORM "windows")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(X86_ARCH "x86_64")
        set(WCONAN_LOCAL_PLATFORM "windows-x64")
    else ()
        set(X86_ARCH "x86")
        set(WCONAN_LOCAL_PLATFORM "windows-x86")
    endif ()
    message(STATUS "WCONAN_PLATFORM: Windows ${X86_ARCH}")
elseif (UNIX)
    set(WCONAN_PLATFORM "linux")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(X86_ARCH "x86_64")
    else ()
        set(X86_ARCH "x86")
    endif ()
    message(STATUS "WCONAN_PLATFORM: Linux ${X86_ARCH}")
    set(LINUX TRUE)
else ()
    message(FATAL_ERROR "unsupported platform")
endif ()

if (NOT DEFINED WCONAN_TARGET_PLATFORM)
    # WCONAN_TARGET_PLATFORM is defined by wconan, e.g.
    #     wconan build -cmake . -p linux-x86_64-gcc-5.4.0_adj
    # the WCONAN_TARGET_PLATFORM is linux-x86_64-gcc-5.4.0_adj
    set(WCONAN_TARGET_PLATFORM ${WCONAN_LOCAL_PLATFORM})
endif()
if (LINUX AND (NOT DEFINED WCONAN_TARGET_PLATFORM))
    message(FATAL_ERROR "build linux but WCONAN_TARGET_PLATFORM is NOT DEFINED, please set(WCONAN_TARGET_PLATFORM <linux_platform>) before include(wconan.cmake)")
endif ()

message(STATUS "CMAKE_CROSSCOMPILING: ${CMAKE_CROSSCOMPILING}")
message(STATUS "WCONAN_TARGET_PLATFORM: ${WCONAN_TARGET_PLATFORM}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_SIZEOF_VOID_P: ${CMAKE_SIZEOF_VOID_P}")

function(wconan_get_libraries_dir out_libraries_dir download_dir)
    set(libraries_dir "${CMAKE_CURRENT_SOURCE_DIR}/../WeChat/${download_dir}/${CMAKE_BUILD_TYPE}/${WCONAN_TARGET_PLATFORM}")
    set(${out_libraries_dir} ${libraries_dir} PARENT_SCOPE)
    message(STATUS "out_libraries_dir ${libraries_dir}")
endfunction()

function(wconan_set_link_directories download_dir)
    if (NOT COMMAND target_link_directories)
        wconan_get_libraries_dir(LIBRARIES_DIR ${download_dir})
        link_directories(${LIBRARIES_DIR}/libs)
    endif()
endfunction()

function(wconan_find_library out_lib_name libs_dir lib_name)
    # try find using original name
    if (EXISTS "${libs_dir}/lib${lib_name}.a" OR
            EXISTS "${libs_dir}/${lib_name}.lib" OR
            EXISTS "${libs_dir}/lib${lib_name}.so" OR
            EXISTS "${libs_dir}/${lib_name}.framework")
        set(${out_lib_name} ${lib_name} PARENT_SCOPE)
        return()
    endif()
    # try find using alternate name
    string(REPLACE "-" "_" alt_lib_name ${lib_name})
    if (NOT alt_lib_name STREQUAL "${lib_name}")
        if (EXISTS "${libs_dir}/lib${alt_lib_name}.a" OR
                EXISTS "${libs_dir}/${alt_lib_name}.lib" OR
                EXISTS "${libs_dir}/lib${alt_lib_name}.so" OR
                EXISTS "${libs_dir}/${alt_lib_name}.framework")
            set(${out_lib_name} ${alt_lib_name} PARENT_SCOPE)
            return()
        endif()
    endif()
    # not found
    set(${out_lib_name} "" PARENT_SCOPE)
endfunction()

#
# import wconan <LIBRARIES> from <DIR> for <TARGET>.
#
# wconan_target_import_libraries(<TARGET> target
#                                <DIR> dir
#                                <LIBRARIES> libs...)
#
function(wconan_target_import_libraries)
    #message(STATUS "ARGC ${ARGC}")
    #message(STATUS "ARGV ${ARGV}")
    #message(STATUS "ARGN ${ARGN}")
    set(prefix WCONAN)
    #set(options IS_A IS_B)
    set(singleValues TARGET DIR)
    set(multiValues LIBRARIES)
    cmake_parse_arguments(PARSE_ARGV 0
            "${prefix}"
            "${options}"
            "${singleValues}"
            "${multiValues}")
    message(STATUS "WCONAN_TARGET ${WCONAN_TARGET}")
    message(STATUS "WCONAN_DIR ${WCONAN_DIR}")
    message(STATUS "WCONAN_LIBRARIES ${WCONAN_LIBRARIES}")

    set(link_libraries)
    wconan_get_libraries_dir(libraries_dir ${WCONAN_DIR})
    set(include_dir ${libraries_dir}/include)
    set(libs_dir ${libraries_dir}/libs)
    set(apple_link_flags -Wl,-F'${libs_dir}')
    target_include_directories(${WCONAN_TARGET} PRIVATE ${include_dir})
    if (COMMAND target_link_directories)
        target_link_directories(${WCONAN_TARGET} PRIVATE ${libs_dir})
    else()
        if (NOT TLD_WARNING)
            set(TLD_WARNING 1)
            message(WARNING "CMake ${CMAKE_VERSION} not support target_link_directories")
        endif()
    endif()
    target_sources(${WCONAN_TARGET} PRIVATE ${SRC_LIB_VERSION})
    foreach (lib_name ${WCONAN_LIBRARIES})
        wconan_find_library(real_lib_name ${libs_dir} ${lib_name})
        if (NOT real_lib_name STREQUAL "")
            if (APPLE AND EXISTS "${libs_dir}/${real_lib_name}.framework")
                list(APPEND link_libraries "-framework ${real_lib_name}")
            else()
                list(APPEND link_libraries ${real_lib_name})
            endif()
            if (real_lib_name STREQUAL ${lib_name})
                message(STATUS "find ${lib_name}, OK")
            else()
                message(STATUS "find ${lib_name} -> ${real_lib_name}, OK")
            endif()
        else()
            string(REPLACE "-" "_" alt_lib_name ${lib_name})
            if (alt_lib_name STREQUAL ${lib_name})
                message(STATUS "find ${lib_name}, NOT FOUND")
            else()
                message(STATUS "find ${lib_name} or ${alt_lib_name}, NOT FOUND")
            endif ()
        endif()
        if (EXISTS "${libs_dir}/${real_lib_name}.framework/Headers")
            #            file(GLOB headers_list "${libs_dir}/${real_lib_name}.framework/Headers")
            #            list(LENGTH headers_list headers_count)
            #            if (headers_count NOT EQUAL 0)
            #                # Headers dir is not empty
            #            endif ()
            target_include_directories(${WCONAN_TARGET} PRIVATE "${libs_dir}/${real_lib_name}.framework")
        endif()
    endforeach()
    target_link_libraries(${WCONAN_TARGET} ${link_libraries})
    if (MACOS)
        set(apple_link_flags "${apple_link_flags} -Wl,-rpath,@executable_path/../Frameworks -Wl,-rpath,@loader_path/Frameworks")
    endif ()
    message(STATUS "apple_link_flags ${apple_link_flags}")
    if (APPLE)
        set_target_properties(${WCONAN_TARGET} PROPERTIES
                LINK_FLAGS "${apple_link_flags}"
                )
    endif()
endfunction()