cmake_minimum_required(VERSION 3.13)

cmake_policy(SET CMP0076 NEW)
#cmake_policy(SET CMP0015 NEW)

####################
# options
####################
option(CFD_SHARED "force shared build (ON or OFF. default:OFF)" OFF)
if(CMAKE_JS_INC)
option(ENABLE_SHARED "enable shared library (ON or OFF. default:ON)" ON)
else()
option(ENABLE_SHARED "enable shared library (ON or OFF. default:OFF)" OFF)
endif()
# use "cmake -DCMAKE_BUILD_TYPE=Debug" or "cmake-js -D"
# option(ENABLE_DEBUG "enable debugging (ON or OFF. default:OFF)" OFF)
option(ENABLE_ELEMENTS "enable elements code (ON or OFF. default:ON)" ON)
option(ENABLE_BITCOIN  "enable bitcoin code (ON or OFF. default:ON)" ON)
option(ENABLE_CAPI  "enable c-api (ON or OFF. default:ON)" ON)

if(NOT WIN32)
#option(TARGET_RPATH "target rpath list (separator is ';') (default:)" "")
set(TARGET_RPATH "" CACHE STRING "target rpath list (separator is ';') (default:)")
option(ENABLE_COVERAGE "enable code coverage (ON or OFF. default:OFF)" OFF)
else()
set(TARGET_RPATH "")
set(ENABLE_COVERAGE FALSE)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
set(ENABLE_DEBUG  TRUE)
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:DEBUGBUILD>)
if(ENABLE_COVERAGE)
set(STACK_PROTECTOR_OPT  "")
else()
set(STACK_PROTECTOR_OPT  $<IF:$<CXX_COMPILER_ID:MSVC>,/GS,-fstack-check -fstack-protector>)
endif()
else()
set(ENABLE_DEBUG  FALSE)
set(STACK_PROTECTOR_OPT  "")
endif() # CMAKE_BUILD_TYPE

if(CFD_SHARED AND (WIN32 OR APPLE))
set(USE_CFD_SHARED  TRUE)
else()
set(USE_CFD_SHARED  FALSE)
endif()

####################
# common setting
####################
set(WORK_WINDOWS_BINARY_DIR_NAME  $<IF:$<CONFIG:Debug>,Debug,Release>)
if(NOT CFD_OBJ_BINARY_DIR)
set(CFD_OBJ_BINARY_DIR   ${CMAKE_BINARY_DIR}/${WORK_WINDOWS_BINARY_DIR_NAME})
set(CFD_ROOT_BINARY_DIR  ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CFD_OBJ_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CFD_OBJ_BINARY_DIR}) # for linux
endif()

if(NOT CFD_SRC_ROOT_DIR)
set(CFD_SRC_ROOT_DIR   ${CMAKE_SOURCE_DIR})
endif()

if(NOT ENABLE_BITCOIN)
set(CFD_BITCOIN_USE   CFD_DISABLE_BITCOIN)
else()
set(CFD_BITCOIN_USE   "")
endif()

if(NOT ENABLE_ELEMENTS)
set(CFD_ELEMENTS_USE   CFD_DISABLE_ELEMENTS)
else()
set(CFD_ELEMENTS_USE   "")
endif()

if(NOT ENABLE_CAPI)
set(CFD_CAPI_USE   CFD_DISABLE_CAPI)
else()
set(CFD_CAPI_USE   "")
endif()

if(ENABLE_SHARED)
set(CFD_CORE_SHARED_OPT  CFD_CORE_SHARED=1)  # msvc only
else()
set(CFD_CORE_SHARED_OPT  "")
endif()

if(NOT WIN32)
if(APPLE)
set(CMAKE_MACOSX_RPATH 1)
endif()
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(TARGET_RPATH)
set(CMAKE_INSTALL_RPATH "${TARGET_RPATH};./;@rpath")
else()
set(CMAKE_INSTALL_RPATH "./;@rpath")
endif()
endif()

if(ENABLE_DEBUG AND ENABLE_COVERAGE)
set(COLLECT_COVERAGE ON)
set(PROFILE_ARCS_OPT -fprofile-arcs)
set(TEST_COVERAGE_OPT -ftest-coverage)
set(GCOV_LIBRARY gcov)
set(COVERAGE 1)
else()
set(PROFILE_ARCS_OPT  "")
set(TEST_COVERAGE_OPT "")
set(GCOV_LIBRARY  "")
endif()

####################
# cfd
####################
transform_makefile_srclist("Makefile.srclist" "${CMAKE_CURRENT_BINARY_DIR}/Makefile.srclist.cmake")
include(${CMAKE_CURRENT_BINARY_DIR}/Makefile.srclist.cmake)

project(cfd CXX)
find_package(cfdcore)
if((NOT cfdcore_FOUND) OR (NOT ${cfdcore_FOUND}))
set(CFDCORE_LIBRARY cfdcore)
set(LIBWALLY_LIBRARY wally)
set(UNIVALUE_LIBRARY univalue)

else()
message(STATUS "[cfdcore_FOUND]=${cfdcore_FOUND}")
message(STATUS "[cfdcore_DIR]=${cfdcore_DIR}")

set(CFDCORE_LIBRARY cfdcore)
set(LIBWALLY_LIBRARY wally)
set(UNIVALUE_LIBRARY univalue)
endif()


if(MSVC)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi")
else()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(ENABLE_SHARED OR USE_CFD_SHARED)
add_library(${PROJECT_NAME} SHARED)
else()
add_library(${PROJECT_NAME} STATIC)
endif()

if(NOT ENABLE_CAPI)
target_sources(${PROJECT_NAME}
  PRIVATE
    ${CFD_SOURCES}
)
else()
target_sources(${PROJECT_NAME}
  PRIVATE
    ${CFD_SOURCES} ${CFD_CAPI_SOURCES}
)
endif()

target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<IF:$<CXX_COMPILER_ID:MSVC>,
      /LD /source-charset:utf-8 /Wall
      /wd4061 /wd4244 /wd4251 /wd4365 /wd4464 /wd4514 /wd4571 /wd4574 /wd4623 /wd4625 /wd4626 /wd4710 /wd4711 /wd4774 /wd4820 /wd4946 /wd5026 /wd5027 /wd5039 /wd5045 /wd5052
      ${STACK_PROTECTOR_OPT},
      -Wall -Wextra ${PROFILE_ARCS_OPT} ${TEST_COVERAGE_OPT}
    >
    $<$<BOOL:$<CXX_COMPILER_ID:GNU>>:${STACK_PROTECTOR_OPT}>
)
#     -fstack-protector
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    CFD_BUILD=1
    ${CFD_BITCOIN_USE}
    ${CFD_ELEMENTS_USE}
    ${CFD_CAPI_USE}
    ${CFD_CORE_SHARED_OPT}
)

if((NOT cfdcore_FOUND) OR (NOT ${cfdcore_FOUND}))
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    .
    ${CFD_SRC_ROOT_DIR}/external/cfd-core/src/include
)
target_link_directories(${PROJECT_NAME}
  PRIVATE
    ./
)
set(USE_INSTALLED_LIBRARY  FALSE)
else()
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    .
    ${cfdcore_DIR}/../include
    ${CFD_SRC_ROOT_DIR}/external/cfd-core/src/include
)
target_link_directories(${PROJECT_NAME}
  PRIVATE
    ./
    ${cfdcore_DIR}/../lib
)
set(USE_INSTALLED_LIBRARY  TRUE)
endif()

target_link_libraries(${PROJECT_NAME}
  PRIVATE $<$<BOOL:$<CXX_COMPILER_ID:GNU>>:-static-libgcc>
  PRIVATE $<$<BOOL:$<CXX_COMPILER_ID:GNU>>:-static-libstdc++>
  PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:pthread>
  PRIVATE
    ${CFDCORE_LIBRARY}
    ${LIBWALLY_LIBRARY}
    ${UNIVALUE_LIBRARY}
    ${GCOV_LIBRARY}
)


if(USE_INSTALLED_LIBRARY)
set(INSTALLED_LIBRARY_DIR  ${cfdcore_DIR}/../lib)
if(ENABLE_SHARED)
set(LIB_PREFIX  ${CMAKE_SHARED_LIBRARY_PREFIX})
set(LIB_SUFFIX  ${CMAKE_SHARED_LIBRARY_SUFFIX})
else()
set(LIB_PREFIX  ${CMAKE_STATIC_LIBRARY_PREFIX})
set(LIB_SUFFIX  ${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set(LIBWALLY_LIB_PATH  ${INSTALLED_LIBRARY_DIR}/${LIB_PREFIX}wally${LIB_SUFFIX})
set(UNIVALUE_LIB_PATH  ${INSTALLED_LIBRARY_DIR}/${LIB_PREFIX}univalue${LIB_SUFFIX})
set(CFDCORE_LIB_PATH   ${INSTALLED_LIBRARY_DIR}/${LIB_PREFIX}cfdcore${LIB_SUFFIX})

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy  ${LIBWALLY_LIB_PATH} ${CFD_OBJ_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy  ${UNIVALUE_LIB_PATH} ${CFD_OBJ_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy  ${CFDCORE_LIB_PATH} ${CFD_OBJ_BINARY_DIR}
)
endif()   # USE_INSTALLED_LIBRARY


####################
# coverage
####################
if(ENABLE_DEBUG AND ENABLE_COVERAGE)
add_custom_target(
  lcov_cfd00
  ${CMAKE_COMMAND} -E make_directory ${CFD_ROOT_BINARY_DIR}/lcov_cfd_output
  COMMAND ${CMAKE_COMMAND} -E remove -f cfd1.info cfd_api.info cfd_cfd.info cfd_cfd1.info cfd.info capi/cfd2.info capi/cfd_capi.info
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/cfd.dir
)
add_custom_target(
  lcov_cfd11
  lcov -c -d . -o cfd1.info
  DEPENDS lcov_cfd00
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/cfd.dir
)
add_custom_target(
  lcov_cfd12
  lcov -o cfd_api.info -e cfd1.info "*/cfdapi_*"
  COMMAND lcov -o cfd_cfd.info -e cfd1.info "*/cfd_*"
  DEPENDS lcov_cfd11
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/cfd.dir
)
add_custom_target(
  lcov_cfd21
  lcov -c -d . -o cfd2.info
  COMMAND lcov -o cfd_capi.info -e cfd2.info "*/cfdcapi_*"
  DEPENDS lcov_cfd00
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/cfd.dir/capi
)
add_custom_target(
  lcov_cfd1
  COMMAND lcov -a cfd_cfd.info -a cfd_api.info -o cfd_cfd1.info
  DEPENDS lcov_cfd12 lcov_cfd21
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/cfd.dir
)
#  COMMAND gcov -f -m *.gcda > console.log
add_custom_target(
  lcov_cfd
  lcov -a cfd_cfd1.info -a capi/cfd_capi.info -o cfd.info
  COMMAND genhtml -s --demangle-cpp -o ${CFD_ROOT_BINARY_DIR}/lcov_cfd_output cfd.info
  DEPENDS lcov_cfd1
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/cfd.dir
)
endif()
# lcov -c -d . -o cfd1.info
# lcov -e cfd1.info "*/cfd/*" -o cfd.info
# genhtml -s --demangle-cpp -o output cfd.info


####################
# install & export
####################
include(GNUInstallDirs)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON) 

install(TARGETS       ${PROJECT_NAME}
        EXPORT        ${PROJECT_NAME}-export
        ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME       DESTINATION ${CMAKE_INSTALL_LIBDIR}
        OBJECTS       DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${PROJECT_NAME}-export
        FILE ${PROJECT_NAME}-config.cmake
        DESTINATION "cmake/"
        EXPORT_LINK_INTERFACE_LIBRARIES)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../include/cfd"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING PATTERN "*.h")

if(ENABLE_CAPI)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../include/cfdc"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING PATTERN "*.h")
endif()

if(MSVC)
install(DIRECTORY "${CFD_OBJ_BINARY_DIR}/"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        FILES_MATCHING PATTERN "cfd.exp")
endif()

#export(TARGETS ${PROJECT_NAME}
#       APPEND FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake)
