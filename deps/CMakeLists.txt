# ----------------------------------------------------------------------- #
# -------------------- Set CMake version and project -------------------- #
# ----------------------------------------------------------------------- #
cmake_minimum_required(VERSION 3.11)
# ----------------------------------------------------------------------- #
# ----------------------- Setup Dependency Helper ----------------------- #
# ----------------------------------------------------------------------- #
macro(metis_setup_dependency directory dependency)
  configure_file(external_${dependency}.txt.in ${dependency}/CMakeLists.txt
                 @ONLY)

  execute_process(
    COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dependency})
  if(result)
    message(FATAL_ERROR "CMake step for ${dependency} failed: ${result}")
  endif()

  if(NOT EXISTS ${directory}/${dependency}/src/${dependency}-install)
    execute_process(
      COMMAND ${CMAKE_COMMAND} --build .
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dependency})
    if(result)
      message(FATAL_ERROR "Build step for ${dependency} failed: ${result}")
    endif()
  endif()

  set(CMAKE_PREFIX_PATH ${directory}/${dependency}/src/${dependency}-install
                        ${CMAKE_PREFIX_PATH})
  set(CMAKE_PREFIX_PATH
      ${CMAKE_PREFIX_PATH}
      PARENT_SCOPE)
endmacro()
# ----------------------------------------------------------------------- #
# ------------------------ Set Dependency Directory --------------------- #
# ----------------------------------------------------------------------- #
if(NOT METIS_BUILD_DEPS_DIR)
  set(METIS_BUILD_DEPS_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif()
# ----------------------------------------------------------------------- #
# -------------------------- Setup Dependency --------------------------- #
# ----------------------------------------------------------------------- #
metis_setup_dependency(${METIS_BUILD_DEPS_DIR} qt)
metis_setup_dependency(${METIS_BUILD_DEPS_DIR} grpc)
# ----------------------------------------------------------------------- #
