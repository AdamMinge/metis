# ----------------------------------------------------------------------- #
# ---------------------------- Set CMake Version ------------------------ #
# ----------------------------------------------------------------------- #
cmake_minimum_required(VERSION 3.11)
# ----------------------------------------------------------------------- #
# ---------------------------- Set files variable ----------------------- #
# ----------------------------------------------------------------------- #
set(source_root ${METIS_SOURCE_DIR}/plugins/src/migrate_v1.0.0_to_v2.0.0)
set(include_root ${METIS_SOURCE_DIR}/plugins/include/migrate_v1.0.0_to_v2.0.0)
set(proto_root ${METIS_SOURCE_DIR}/protos)

set(sources
    ${source_root}/plugin.cpp
    ${source_root}/migrator.cpp)

set(includes
    ${include_root}/export.h
    ${include_root}/plugin.h
    ${include_root}/migrator.h)

set(protos 
    ${proto_root}/snap_v1.0.0.proto
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
  migrate_v1_0_0_to_v2_0_0
  SOURCES
  ${sources}
  ${includes}
  PROTOS
  ${protos}
  DEPENDS
  Qt6::Core
  metis::metis)

target_include_directories(
  migrate_v1_0_0_to_v2_0_0
  PUBLIC $<BUILD_INTERFACE:${METIS_SOURCE_DIR}/plugins/include>
  INTERFACE $<INSTALL_INTERFACE:include>
  PRIVATE ${METIS_SOURCE_DIR}/plugins/src)
# ----------------------------------------------------------------------- #
