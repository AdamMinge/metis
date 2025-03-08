# ----------------------------------------------------------------------- #
# ---------------------------- Set CMake Version ------------------------ #
# ----------------------------------------------------------------------- #
cmake_minimum_required(VERSION 3.11)
# ----------------------------------------------------------------------- #
# ---------------------------- Set files variable ----------------------- #
# ----------------------------------------------------------------------- #
set(source_root ${METIS_SOURCE_DIR}/apps/src/differ)
set(include_root ${METIS_SOURCE_DIR}/apps/include/differ)
set(ui_root ${METIS_SOURCE_DIR}/apps/ui/differ)
set(resource_root ${METIS_SOURCE_DIR}/apps/resource/differ)

set(sources
    ${source_root}/main.cpp
    ${source_root}/main_window.cpp
    ${source_root}/differ_widget.cpp
    ${source_root}/console_widget.cpp)

set(includes
    ${include_root}/resources.h
    ${include_root}/main_window.h
    ${include_root}/differ_widget.h
    ${include_root}/console_widget.h)

set(uis
    ${ui_root}/main_window.ui
    ${ui_root}/differ_widget.ui)

set(resources ${resource_root}/differ.qrc)
# ----------------------------------------------------------------------- #
# ------------------------- Qt libraries configuration ------------------ #
# ----------------------------------------------------------------------- #
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${ui_root})
# ----------------------------------------------------------------------- #
# -------------------------- Find external libraries -------------------- #
# ----------------------------------------------------------------------- #
find_package(Qt6 REQUIRED COMPONENTS Widgets)
# ----------------------------------------------------------------------- #
# ---------------------------- Create app differ ------------------------ #
# ----------------------------------------------------------------------- #
metis_add_application(
  differ
  SOURCES
  ${sources}
  ${includes}
  ${uis}
  ${resources}
  DEPENDS
  Qt6::Widgets
  metis::metis)

target_include_directories(
  differ
  PUBLIC $<BUILD_INTERFACE:${include_root}>
  INTERFACE $<INSTALL_INTERFACE:include>
  PRIVATE ${source_root})
# ----------------------------------------------------------------------- #
