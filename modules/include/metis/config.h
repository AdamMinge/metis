/* -------------------------------------------------------------------------- */
#ifndef METIS_CONFIG_H
#define METIS_CONFIG_H
/* -------------------------------------------------------------------------- */
/* ---------------------------- Define METIS version ----------------------- */
/* -------------------------------------------------------------------------- */
// clang-format off

#define METIS_VERSION_MAJOR 0
#define METIS_VERSION_MINOR 1
#define METIS_VERSION_PATCH 0
#define METIS_VERSION 0.1.0
#define METIS_VERSION_STR "0.1.0"

// clang-format on
/* -------------------------------------------------------------------------- */
/* ----------------------- Identify the operating system -------------------- */
/* -------------------------------------------------------------------------- */
#if defined(_WIN32)
#define METIS_OS_WINDOWS
#elif defined(__linux__)
#define METIS_OS_LINUX
#else
#error This operating system is not supported by METIS library
#endif
/* -------------------------------------------------------------------------- */
/* ------------------------ Define a portable debug macro ------------------- */
/* -------------------------------------------------------------------------- */
#if !defined(NDEBUG)
#define METIS_DEBUG
#endif
/* -------------------------------------------------------------------------- */
/* ----------------------- Define a export/import dll macro ----------------- */
/* -------------------------------------------------------------------------- */
#if !defined(METIS_STATIC)

#if defined(METIS_OS_WINDOWS)

#define METIS_API_EXPORT __declspec(dllexport)
#define METIS_API_IMPORT __declspec(dllimport)

#ifdef _MSC_VER

#pragma warning(disable : 4251)

#endif

#else

#define METIS_API_EXPORT __attribute__((__visibility__("default")))
#define METIS_API_IMPORT __attribute__((__visibility__("default")))

#endif

#else

#define METIS_API_EXPORT
#define METIS_API_IMPORT

#endif
/* -------------------------------------------------------------------------- */
/* -------------------- Define helpers to set items as deprecated ----------- */
/* -------------------------------------------------------------------------- */
#if !defined(METIS_DEPRECATED_WARNINGS)
#define METIS_DEPRECATED [[deprecated]]
#else
#define METIS_DEPRECATED
#endif
/* -------------------------------------------------------------------------- */
#endif  // METIS_CONFIG_H
/* -------------------------------------------------------------------------- */
