# ----------------------------------------------------------------------- #
# ---------------------------- Set CMake Version ------------------------ #
# ----------------------------------------------------------------------- #
cmake_minimum_required(VERSION 3.11)
# ----------------------------------------------------------------------- #
# ---------------------------- Set files variable ----------------------- #
# ----------------------------------------------------------------------- #
set(source_root ${METIS_SOURCE_DIR}/modules/src/metis)
set(include_root ${METIS_SOURCE_DIR}/modules/include/metis)
set(resource_root ${METIS_SOURCE_DIR}/modules/resource/metis)

set(sources
    ${source_root}/application.cpp
    ${source_root}/command_line_parser.cpp
    ${source_root}/plugin.cpp
    ${source_root}/logging_manager.cpp
    ${source_root}/action_manager.cpp
    ${source_root}/plugin_interface.cpp
    ${source_root}/preferences_manager.cpp
    ${source_root}/plugin_manager.cpp
    ${source_root}/migrator.cpp
    ${source_root}/differ.cpp)

set(includes
    ${include_root}/config.h
    ${include_root}/export.h
    ${include_root}/application.h
    ${include_root}/command_line_parser.h
    ${include_root}/action_manager.h
    ${include_root}/logging_manager.h
    ${include_root}/plugin.h
    ${include_root}/plugin_listener.h
    ${include_root}/plugin_interface.h
    ${include_root}/plugin_manager.h
    ${include_root}/preferences_manager.h
    ${include_root}/migrator.h
    ${include_root}/differ.h)

set(resources)
# ----------------------------------------------------------------------- #
# ------------------------- Qt libraries configuration ------------------ #
# ----------------------------------------------------------------------- #
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
# ----------------------------------------------------------------------- #
# -------------------------- Find external libraries -------------------- #
# ----------------------------------------------------------------------- #
find_package(Qt6 REQUIRED COMPONENTS Widgets)
# ----------------------------------------------------------------------- #
# -------------------------- Create metis shared lib --------------------- #
# ----------------------------------------------------------------------- #
metis_add_module(
  metis
  SOURCES
  ${sources}
  ${includes}
  ${resources}
  DEPENDS
  Qt6::Widgets
  DEPENDS_PRIVATE)

target_include_directories(
  metis
  PUBLIC $<BUILD_INTERFACE:${METIS_SOURCE_DIR}/modules/include/>
  INTERFACE $<INSTALL_INTERFACE:include>
  PRIVATE ${METIS_SOURCE_DIR}/modules/src/)
# ----------------------------------------------------------------------- #
