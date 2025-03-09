# ----------------------------------------------------------------------- #
# ---------------------------- Set CMake Version ------------------------ #
# ----------------------------------------------------------------------- #
cmake_minimum_required(VERSION 3.11)
# ----------------------------------------------------------------------- #
# ---------------------------- Set files variable ----------------------- #
# ----------------------------------------------------------------------- #
set(source_root ${METIS_SOURCE_DIR}/plugins/src/diff_v2.0.0)
set(include_root ${METIS_SOURCE_DIR}/plugins/include/diff_v2.0.0)
set(proto_root ${METIS_SOURCE_DIR}/protos)

set(sources
    ${source_root}/plugin.cpp
    ${source_root}/differ.cpp)

set(includes
    ${include_root}/plugin.h
    ${include_root}/differ.h)

set(protos 
    ${proto_root}/snap_v2.0.0.proto)
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
  diff_v2_0_0
  SOURCES
  ${sources}
  ${includes}
  PROTOS
  ${protos}
  DEPENDS
  Qt6::Core
  metis::metis)

target_include_directories(
  diff_v2_0_0
  PUBLIC $<BUILD_INTERFACE:${METIS_SOURCE_DIR}/plugins/include>
  INTERFACE $<INSTALL_INTERFACE:include>
  PRIVATE ${METIS_SOURCE_DIR}/plugins/src)
# ----------------------------------------------------------------------- #
