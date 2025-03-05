# ----------------------------------------------------------------------- #
# ---------------------------- Set CMake Version ------------------------ #
# ----------------------------------------------------------------------- #
cmake_minimum_required(VERSION 3.11)
# ----------------------------------------------------------------------- #
# ---------------------------- Set files variable ----------------------- #
# ----------------------------------------------------------------------- #
set(source_root ${METIS_SOURCE_DIR}/apps/src/migrator)
set(include_root ${METIS_SOURCE_DIR}/apps/include/migrator)
set(ui_root ${METIS_SOURCE_DIR}/apps/ui/migrator)
set(resource_root ${METIS_SOURCE_DIR}/apps/resource/migrator)

set(sources
    ${source_root}/main.cpp
    ${source_root}/main_window.cpp
    ${source_root}/migrator_widget.cpp
    ${source_root}/console_widget.cpp
    ${source_root}/migration_manager.cpp)

set(includes
    ${include_root}/resources.h
    ${include_root}/main_window.h
    ${include_root}/migrator_widget.h
    ${include_root}/console_widget.h
    ${include_root}/migration_manager.h)

set(uis
    ${ui_root}/main_window.ui
    ${ui_root}/migrator_widget.ui)

set(resources ${resource_root}/migrator.qrc)
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
# --------------------------- Create app migrator ----------------------- #
# ----------------------------------------------------------------------- #
metis_add_application(
  migrator
  SOURCES
  ${sources}
  ${includes}
  ${uis}
  ${resources}
  DEPENDS
  Qt6::Widgets
  metis::metis)

target_include_directories(
  migrator
  PUBLIC $<BUILD_INTERFACE:${include_root}>
  INTERFACE $<INSTALL_INTERFACE:include>
  PRIVATE ${source_root})
# ----------------------------------------------------------------------- #
