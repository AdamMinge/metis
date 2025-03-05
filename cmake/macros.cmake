# ----------------------------------------------------------------------- #
# -------------- Define a macro that prepare target module -------------- #
# ----------------------------------------------------------------------- #
macro(_metis_prepare_module_target target)
  string(REPLACE "-" "_" NAME_UPPER "${target}")
  string(TOUPPER "${NAME_UPPER}" NAME_UPPER)
  set_target_properties(${target} PROPERTIES DEFINE_SYMBOL
                                             ${NAME_UPPER}_EXPORTS)

  if(BUILD_SHARED_LIBS)
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)
  else()
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -s-d)
    set_target_properties(${target} PROPERTIES RELEASE_POSTFIX -s)
  endif()

  set_target_properties(${target} PROPERTIES COMPILE_FEATURES cxx_std_20)
  set_target_properties(${target} PROPERTIES LINKER_LANGUAGE CXX)

  install(
    TARGETS ${target}
    EXPORT metisConfigExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib)

endmacro()
# ----------------------------------------------------------------------- # 
# ---- Define a macro that create proto libs and link it to target ------ #
# ----------------------------------------------------------------------- #
macro(__metis_add_proto_modules target)
  cmake_parse_arguments(THIS "" "" "PROTOS" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling metis_add_proto_modules: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  find_package(protobuf CONFIG REQUIRED)
  find_package(gRPC CONFIG REQUIRED)

  foreach(PROTO_FILE ${THIS_PROTOS})
    get_filename_component(PROTO_FILENAME ${PROTO_FILE} NAME)
    string(REGEX REPLACE "\\.proto$" "" PROTO_FILENAME_WE ${PROTO_FILENAME})
    
    get_filename_component(PROTO_IMPORT_DIRS ${PROTO_FILE} DIRECTORY)

    set(PROTO_TARGET ${target}_${PROTO_FILENAME_WE}_proto)
    set(PROTO_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/${PROTO_FILENAME_WE}_proto)

    add_custom_target(
      ${PROTO_TARGET}_create_proto_dir ALL
      COMMAND ${CMAKE_COMMAND} -E make_directory ${PROTO_BINARY_DIR}
      COMMENT "Creating directory ${PROTO_BINARY_DIR}")

    add_library(${PROTO_TARGET} OBJECT ${PROTO_FILE})
    target_link_libraries(${PROTO_TARGET} PUBLIC protobuf::libprotobuf
                                                 gRPC::grpc++)
    target_include_directories(${PROTO_TARGET}
                               PUBLIC $<BUILD_INTERFACE:${PROTO_BINARY_DIR}/..>)
    add_dependencies(${PROTO_TARGET} ${PROTO_TARGET}_create_proto_dir)
    set_target_properties(${PROTO_TARGET} PROPERTIES POSITION_INDEPENDENT_CODE
                                                     TRUE)

    protobuf_generate(TARGET ${PROTO_TARGET} IMPORT_DIRS ${PROTO_IMPORT_DIRS}
                      PROTOC_OUT_DIR ${PROTO_BINARY_DIR})

    protobuf_generate(
      TARGET
      ${PROTO_TARGET}
      LANGUAGE
      grpc
      GENERATE_EXTENSIONS
      .grpc.pb.h
      .grpc.pb.cc
      PLUGIN
      "protoc-gen-grpc=\$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
      IMPORT_DIRS
      ${PROTO_IMPORT_DIRS}
      PROTOC_OUT_DIR
      ${PROTO_BINARY_DIR})

    target_link_libraries(${target} PRIVATE ${PROTO_TARGET})

  endforeach()

endmacro()
# ----------------------------------------------------------------------- #
# ---------- Define a macro that helps add headers only module ---------- #
# ----------------------------------------------------------------------- #
macro(metis_add_headers_only_module target)

  cmake_parse_arguments(THIS "" "" "SOURCES;DEPENDS" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling metis_add_module: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  add_library(${target} INTERFACE ${THIS_SOURCES})
  add_library(metis::${target} ALIAS ${target})

  _metis_prepare_module_target(${target})

  if(THIS_DEPENDS)
    target_link_libraries(${target} INTERFACE ${THIS_DEPENDS})
  endif()

  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(${target} INTERFACE "QTILS_STATIC")
  endif()

endmacro()
# ----------------------------------------------------------------------- #
# ----------------- Define a macro that helps add module ---------------- #
# ----------------------------------------------------------------------- #
macro(metis_add_module target)
  cmake_parse_arguments(
    THIS
    ""
    ""
    "SOURCES;DEPENDS;DEPENDS_PRIVATE;PRECOMPILE_HEADERS;PRECOMPILE_PRIVATE_HEADERS"
    ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling metis_add_module: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  add_library(${target} ${THIS_SOURCES})
  add_library(metis::${target} ALIAS ${target})

  _metis_prepare_module_target(${target})

  if(THIS_DEPENDS)
    target_link_libraries(${target} PUBLIC ${THIS_DEPENDS})
  endif()

  if(THIS_DEPENDS_PRIVATE)
    target_link_libraries(${target} PRIVATE ${THIS_DEPENDS_PRIVATE})
  endif()

  if(THIS_PRECOMPILE_HEADERS)
    target_precompile_headers(${target} PUBLIC ${THIS_PRECOMPILE_HEADERS})
  endif()

  if(THIS_PRECOMPILE_PRIVATE_HEADERS)
    target_precompile_headers(${target} PRIVATE
                              ${THIS_PRECOMPILE_PRIVATE_HEADERS})
  endif()

  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(${target} PUBLIC "QTILS_STATIC")
  endif()

endmacro()
# ----------------------------------------------------------------------- #
# --------------- Define a macro that helps export engine --------------- #
# ----------------------------------------------------------------------- #
function(metis_export_modules)

  if(BUILD_SHARED_LIBS)
    set(config_name "shared")
  else()
    set(config_name "static")
  endif()

  set(current_dir "${METIS_SOURCE_DIR}/cmake")
  set(targets_config_filename "metis-${config_name}-targets.cmake")
  set(config_package_location ${CMAKE_INSTALL_LIBDIR}/cmake/metis)

  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/metis-config-version.cmake"
    VERSION ${METIS_VERSION_MAJOR}.${METIS_VERSION_MINOR}
    COMPATIBILITY SameMajorVersion)

  export(EXPORT metisConfigExport
         FILE "${CMAKE_CURRENT_BINARY_DIR}/${targets_config_filename}")

  configure_package_config_file(
    "${current_dir}/metis-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/metis-config.cmake"
    INSTALL_DESTINATION "${config_package_location}")
  configure_package_config_file(
    "${current_dir}/metis-config-dependencies.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/metis-config-dependencies.cmake"
    INSTALL_DESTINATION "${config_package_location}")

  install(
    EXPORT metisConfigExport
    FILE ${targets_config_filename}
    NAMESPACE metis::
    DESTINATION ${config_package_location})

  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/metis-config.cmake
          ${CMAKE_CURRENT_BINARY_DIR}/metis-config-dependencies.cmake
          ${CMAKE_CURRENT_BINARY_DIR}/metis-config-version.cmake
    DESTINATION ${config_package_location})

  install(
    DIRECTORY ${METIS_SOURCE_DIR}/modules/include
    DESTINATION .
    COMPONENT devel
    FILES_MATCHING
    PATTERN "*.hpp"
    PATTERN "*.inl"
    PATTERN "*.h")

endfunction()
# ----------------------------------------------------------------------- #
# -------------- Define a macro that helps add application -------------- #
# ----------------------------------------------------------------------- #
macro(_metis_add_executable target)

  cmake_parse_arguments(THIS "" "RESOURCES_DIR"
                        "SOURCES;DEPENDS;DEPENDS_PRIVATE" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling _metis_add_executable: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  add_executable(${target} ${THIS_SOURCES})
  set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)

  if(THIS_DEPENDS)
    target_link_libraries(${target} PUBLIC ${THIS_DEPENDS})
  endif()

  if(THIS_DEPENDS_PRIVATE)
    target_link_libraries(${target} PRIVATE ${THIS_DEPENDS_PRIVATE})
  endif()

  install(TARGETS ${target} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                                    COMPONENT bin)

  if(THIS_RESOURCES_DIR)
    get_filename_component(THIS_RESOURCES_DIR "${THIS_RESOURCES_DIR}" ABSOLUTE)

    if(NOT EXISTS "${THIS_RESOURCES_DIR}")
      message(
        FATAL_ERROR
          "Given resources directory to install does not exist: ${THIS_RESOURCES_DIR}"
      )
    endif()
    install(DIRECTORY ${THIS_RESOURCES_DIR}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/${target})
  endif()

  set_target_properties(${target} PROPERTIES CXX_STANDARD 20)

endmacro()
# ----------------------------------------------------------------------- #
# -------------- Define a macro that helps add engine test -------------- #
# ----------------------------------------------------------------------- #
macro(metis_add_test target)

  cmake_parse_arguments(THIS "" "RESOURCES_DIR"
                        "SOURCES;DEPENDS;DEPENDS_PRIVATE" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling metis_add_test: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  find_package(GTest REQUIRED)

  _metis_add_executable(
    ${target}
    SOURCES
    ${THIS_SOURCES}
    DEPENDS
    ${THIS_DEPENDS}
    DEPENDS_PRIVATE
    GTest::gtest
    GTest::gmock
    ${THIS_DEPENDS_PRIVATE}
    RESOURCES_DIR
    ${THIS_RESOURCES_DIR})

endmacro()
# ----------------------------------------------------------------------- #
# ------------ Define a macro that helps add engine example ------------- #
# ----------------------------------------------------------------------- #
macro(metis_add_example target)

  cmake_parse_arguments(THIS "" "RESOURCES_DIR"
                        "SOURCES;DEPENDS;DEPENDS_PRIVATE" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling metis_add_test: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  _metis_add_executable(
    ${target}
    SOURCES
    ${THIS_SOURCES}
    DEPENDS
    ${THIS_DEPENDS}
    DEPENDS_PRIVATE
    ${THIS_DEPENDS_PRIVATE}
    RESOURCES_DIR
    ${THIS_RESOURCES_DIR})

endmacro()
# ----------------------------------------------------------------------- #
# ----------------- Define a macro that helps add tool ------------------ #
# ----------------------------------------------------------------------- #
macro(metis_add_application target)

  cmake_parse_arguments(THIS "" "RESOURCES_DIR"
                        "SOURCES;DEPENDS;DEPENDS_PRIVATE" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling metis_add_application: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  _metis_add_executable(
    ${target}
    SOURCES
    ${THIS_SOURCES}
    DEPENDS
    ${THIS_DEPENDS}
    DEPENDS_PRIVATE
    ${THIS_DEPENDS_PRIVATE}
    RESOURCES_DIR
    ${THIS_RESOURCES_DIR})

  target_compile_definitions(${target} PUBLIC QT_NO_KEYWORDS)

endmacro()
# ----------------------------------------------------------------------- #
# --------------- Define a macro that helps add utils lib --------------- #
# ----------------------------------------------------------------------- #
macro(metis_add_utils target)

  cmake_parse_arguments(
    THIS
    ""
    ""
    "SOURCES;DEPENDS;DEPENDS_PRIVATE;DEPENDS_TO_EXPORT;PRECOMPILE_HEADERS;PRECOMPILE_PRIVATE_HEADERS"
    ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling metis_add_utils: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  add_library(${target} ${THIS_SOURCES})
  add_library(metis::${target} ALIAS ${target})

  _metis_prepare_module_target(${target})

  if(THIS_DEPENDS)
    target_link_libraries(${target} PUBLIC ${THIS_DEPENDS})
  endif()

  if(THIS_DEPENDS_PRIVATE)
    target_link_libraries(${target} PRIVATE ${THIS_DEPENDS_PRIVATE})
  endif()

  if(THIS_PRECOMPILE_HEADERS)
    target_precompile_headers(${target} PUBLIC ${THIS_PRECOMPILE_HEADERS})
  endif()

  if(THIS_PRECOMPILE_PRIVATE_HEADERS)
    target_precompile_headers(${target} PRIVATE
                              ${THIS_PRECOMPILE_PRIVATE_HEADERS})
  endif()

  foreach(target_depends ${THIS_DEPENDS_TO_EXPORT})
    install(TARGETS ${target_depends} EXPORT metisConfigExport)
  endforeach()

  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(${target} PUBLIC "METIS_STATIC")
  endif()

endmacro()
# ----------------------------------------------------------------------- #
# ---------------- Define a macro that helps add plugins ---------------- #
# ----------------------------------------------------------------------- #
macro(metis_add_plugins target)

  cmake_parse_arguments(
    THIS
    ""
    ""
    "SOURCES;PROTOS;DEPENDS;DEPENDS_PRIVATE;PRECOMPILE_HEADERS;PRECOMPILE_PRIVATE_HEADERS"
    ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling metis_add_utils: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  add_library(${target} ${THIS_SOURCES})
  add_library(plugin::${target} ALIAS ${target})

  _metis_prepare_module_target(${target})

  if(THIS_PROTOS)
    __metis_add_proto_modules(${target} PROTOS ${THIS_PROTOS})
  endif()

  if(THIS_DEPENDS)
    target_link_libraries(${target} PUBLIC ${THIS_DEPENDS})
  endif()

  if(THIS_DEPENDS_PRIVATE)
    target_link_libraries(${target} PRIVATE ${THIS_DEPENDS_PRIVATE})
  endif()

  if(THIS_PRECOMPILE_HEADERS)
    target_precompile_headers(${target} PUBLIC ${THIS_PRECOMPILE_HEADERS})
  endif()

  if(THIS_PRECOMPILE_PRIVATE_HEADERS)
    target_precompile_headers(${target} PRIVATE
                              ${THIS_PRECOMPILE_PRIVATE_HEADERS})
  endif()

  string(REPLACE "-" "_" NAME_UPPER "${target}")
  string(TOUPPER "${NAME_UPPER}" NAME_UPPER)
  set_target_properties(${target} PROPERTIES DEFINE_SYMBOL
                                             ${NAME_UPPER}_EXPORTS)

  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(${target} PUBLIC "METIS_STATIC")
  endif()

endmacro()
# ----------------------------------------------------------------------- #
# ------------- Define a macro that generate documentation -------------- #
# ----------------------------------------------------------------------- #
function(metis_generate_documentation)
  find_package(Doxygen REQUIRED)
  set(DOXYGEN_IN ${METIS_SOURCE_DIR}/docs/Doxyfile.in)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile)

  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

  add_custom_target(
    docs_doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
endfunction()
# ----------------------------------------------------------------------- #
# -------------- Define a macro that install documentation -------------- #
# ----------------------------------------------------------------------- #
function(metis_install_documentation)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc_doxygen/
          DESTINATION ${CMAKE_INSTALL_PREFIX}/docs)
endfunction()
# ----------------------------------------------------------------------- #
# ----------- Define a macro that helps defining translations ----------- #
# ----------------------------------------------------------------------- #
macro(metis_add_translations target)

  cmake_parse_arguments(THIS "" "QM_DIR" "TS_FILES;DIRS" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling metis_add_translations: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  add_custom_target(${target}_update_all_ts_files ALL)
  add_custom_target(${target}_create_all_qm_files ALL)

  find_file(
    LUPDATE_PATH lupdate
    HINTS ${CMAKE_PREFIX_PATH}
    PATH_SUFFIXES bin)

  find_file(
    LRELEASE_PATH lrelease
    HINTS ${CMAKE_PREFIX_PATH}
    PATH_SUFFIXES bin)

  foreach(TS_FILE ${THIS_TS_FILES})

    get_filename_component(I18N_NAME ${TS_FILE} NAME_WE)
    set(TS_TARGET_NAME "update_ts_file_${I18N_NAME}")

    add_custom_target(
      ${TS_TARGET_NAME}
      COMMAND ${LUPDATE_PATH} -I ${THIS_DIRS} -recursive -ts ${TS_FILE}
      VERBATIM)

    add_dependencies(${target}_update_all_ts_files ${TS_TARGET_NAME})
    set(QM_TARGET_NAME "create_qm_file_${I18N_NAME}")
    set(QM_FILE "${THIS_QM_DIR}/${I18N_NAME}.qm")
    add_custom_target(
      ${QM_TARGET_NAME}
      COMMAND ${LRELEASE_PATH} ${TS_FILE} -qm ${QM_FILE}
      VERBATIM)

    add_dependencies(${QM_TARGET_NAME} ${TS_TARGET_NAME})
    add_dependencies(${target}_create_all_qm_files ${QM_TARGET_NAME})
  endforeach()

  add_dependencies(${target} ${target}_create_all_qm_files)

endmacro()
# ----------------------------------------------------------------------- #
# ----------- Define a function that helps add static analysis ---------- #
# ----------------------------------------------------------------------- #
function(metis_static_analyzers)
  if(METIS_ENABLE_CLANG_TIDY)
    find_program(CLANGTIDY clang-tidy)
    if(CLANGTIDY)
      set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY}
                               -extra-arg=-Wno-unknown-warning-option)
    else()
      message(FATAL_ERROR "Clang-Tidy requested but executable not found")
    endif()
  endif()

  if(METIS_ENABLE_CPPCHECK)
    find_program(CPPCHECK cppcheck)
    if(CPPCHECK)
      set(CMAKE_CXX_CPPCHECK ${CPPCHECK} --suppress=missingInclude --enable=all
                             --inline-suppr --inconclusive --force)
    else()
      message(FATAL_ERROR "Cppcheck requested but executable not found")
    endif()
  endif()
endfunction()
# ----------------------------------------------------------------------- # #
