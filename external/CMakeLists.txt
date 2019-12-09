project(external_download NONE)

# load file
set(EXTERNAL_DEBUG_FILENAME  local_resource/external_project_local_setting.config)
set(DEBUG_VERSION_FILE  ${CMAKE_SOURCE_DIR}/${EXTERNAL_DEBUG_FILENAME})
if(EXISTS ${DEBUG_VERSION_FILE})
transform_makefile_srclist(${DEBUG_VERSION_FILE} "${CMAKE_CURRENT_BINARY_DIR}/${EXTERNAL_DEBUG_FILENAME}.cmake")
include(${CMAKE_CURRENT_BINARY_DIR}/${EXTERNAL_DEBUG_FILENAME}.cmake)
endif()

# cfd-core
if(CFDCORE_TARGET_VERSION)
set(CFDCORE_TARGET_TAG  ${CFDCORE_TARGET_VERSION})
message(STATUS "[external project local] cfd-core target=${CFDCORE_TARGET_VERSION}")
else()
set(CFDCORE_TARGET_TAG  v0.0.2)
endif()
if(CFDCORE_TARGET_URL)
set(CFDCORE_TARGET_REP  ${CFDCORE_TARGET_URL})
message(STATUS "[external project local] cfd-core url=${CFDCORE_TARGET_URL}")
else()
set(CFDCORE_TARGET_REP  https://github.com/p2pderivatives/cfd-core.git)
endif()

set(TEMPLATE_PROJECT_NAME           cfd-core)
set(TEMPLATE_PROJECT_GIT_REPOSITORY ${CFDCORE_TARGET_REP})
set(TEMPLATE_PROJECT_GIT_TAG        ${CFDCORE_TARGET_TAG})
set(PROJECT_EXTERNAL  "${CMAKE_SOURCE_DIR}/external/${TEMPLATE_PROJECT_NAME}/external")
set(DIR_PATH "${CFD_ROOT_BINARY_DIR}/${TEMPLATE_PROJECT_NAME}")
set(DL_PATH "${CFD_ROOT_BINARY_DIR}/external/${TEMPLATE_PROJECT_NAME}/download")

get_property(PROP_VALUE  GLOBAL  PROPERTY ${TEMPLATE_PROJECT_NAME})
if(PROP_VALUE)
  message(STATUS "[exist directory] ${TEMPLATE_PROJECT_NAME} exist")
else()
configure_file(template_CMakeLists.txt.in ${DL_PATH}/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -S . -B ${DL_PATH}
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${DL_PATH} )
if(result)
  message(FATAL_ERROR "CMake step for ${TEMPLATE_PROJECT_NAME} failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build ${DL_PATH}
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${DL_PATH} )
if(result)
  message(FATAL_ERROR "Build step for ${TEMPLATE_PROJECT_NAME} failed: ${result}")
endif()
if(EXISTS ${PROJECT_EXTERNAL})
  message(STATUS "[check exist directory] exist ${PROJECT_EXTERNAL}")
  message(STATUS "[check exist directory] dirpath ${DIR_PATH}")
  add_subdirectory(${CMAKE_SOURCE_DIR}/external/${TEMPLATE_PROJECT_NAME}/external
                   ${CFD_ROOT_BINARY_DIR}/external/${TEMPLATE_PROJECT_NAME}/external)
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/external/${TEMPLATE_PROJECT_NAME}
                 ${DIR_PATH}/build)
set_property(GLOBAL PROPERTY ${TEMPLATE_PROJECT_NAME} 1)
endif()


# googletest
if(ENABLE_TESTS)
if(GTEST_TARGET_VERSION)
set(GTEST_TARGET_TAG  ${GTEST_TARGET_VERSION})
message(STATUS "[external project local] google-test target=${GTEST_TARGET_VERSION}")
else()
set(GTEST_TARGET_TAG  release-1.8.1)
endif()

set(TEMPLATE_PROJECT_NAME           googletest)
set(TEMPLATE_PROJECT_GIT_REPOSITORY https://github.com/google/googletest.git)
set(TEMPLATE_PROJECT_GIT_TAG        ${GTEST_TARGET_TAG})
set(PROJECT_EXTERNAL  "${CMAKE_SOURCE_DIR}/external/${TEMPLATE_PROJECT_NAME}/external")
set(DL_PATH "${CFD_ROOT_BINARY_DIR}/external/${TEMPLATE_PROJECT_NAME}/download")

get_property(PROP_VALUE  GLOBAL  PROPERTY ${TEMPLATE_PROJECT_NAME})
if(PROP_VALUE)
  message(STATUS "[exist directory] ${TEMPLATE_PROJECT_NAME} exist")
else()
configure_file(googletest_CMakeLists.txt.in ${DL_PATH}/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -S . -B ${DL_PATH}
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${DL_PATH} )
if(result)
  message(FATAL_ERROR "CMake step for ${TEMPLATE_PROJECT_NAME} failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build ${DL_PATH}
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${DL_PATH} )
if(result)
  message(FATAL_ERROR "Build step for ${TEMPLATE_PROJECT_NAME} failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/${TEMPLATE_PROJECT_NAME}
                 ${CFD_ROOT_BINARY_DIR}/${TEMPLATE_PROJECT_NAME}/build)
set_property(GLOBAL PROPERTY ${TEMPLATE_PROJECT_NAME} 1)
endif()
endif() # MSVC OR ENABLE_TESTS
