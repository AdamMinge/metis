# ----------------------------------------------------------------------- #
# ---------------------------- Set CMake Version ------------------------ #
# ----------------------------------------------------------------------- #
cmake_minimum_required(VERSION 3.11)
# ----------------------------------------------------------------------- #
# ---------------------------- Set files variable ----------------------- #
# ----------------------------------------------------------------------- #
set(source_root ${METIS_SOURCE_DIR}/plugins/src/diff_v3.0.0)
set(include_root ${METIS_SOURCE_DIR}/plugins/include/diff_v3.0.0)
set(resource_root ${METIS_SOURCE_DIR}/plugins/resource/diff_v3.0.0)
set(proto_root ${METIS_SOURCE_DIR}/protos)

set(sources
    ${source_root}/plugin.cpp
    ${source_root}/differ.cpp)

set(includes
    ${include_root}/plugin.h
    ${include_root}/differ.h)

set(resources ${resource_root}/diff_v3.0.0.qrc)

set(protos 
    ${proto_root}/snap_v3.0.0.proto)
# ----------------------------------------------------------------------- #
# ------------------------- Qt libraries configuration ------------------ #
# ----------------------------------------------------------------------- #
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
# ----------------------------------------------------------------------- #
# -------------------------- Find external libraries -------------------- #
# ----------------------------------------------------------------------- #
find_package(Qt6 REQUIRED COMPONENTS Core)
# ----------------------------------------------------------------------- #
# -------------------------- Create plugin shared lib ------------------- #
# ----------------------------------------------------------------------- #
metis_add_plugins(
  diff_v3_0_0
  SOURCES
  ${sources}
  ${includes}
  ${resources}
  PROTOS
  ${protos}
  DEPENDS
  Qt6::Core
  metis::metis)

target_include_directories(
  diff_v3_0_0
  PUBLIC $<BUILD_INTERFACE:${METIS_SOURCE_DIR}/plugins/include>
  INTERFACE $<INSTALL_INTERFACE:include>
  PRIVATE ${METIS_SOURCE_DIR}/plugins/src)
# ----------------------------------------------------------------------- #
