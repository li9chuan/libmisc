#-----------------------------------------------------------------------------
#
# NLMISC
#  Authors: Nevrax and the Ryzom Core Community
#  Version: 0.12.0

#-----------------------------------------------------------------------------
# Load some macros.
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
#-----------------------------------------------------------------------------
# Set CMake 2.6 Policies.
IF(COMMAND cmake_policy)
  # Works around warnings libraries linked against that don't
  # have absolute paths (e.g. -lpthread)
  cmake_policy(SET CMP0003 NEW)

  # allow to link to qtmain automatically under Windows
  IF(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
  ENDIF()
ENDIF()

INCLUDE(nel)
INCLUDE(ConfigureChecks)
INCLUDE(CheckDepends)
INCLUDE(${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)

# Force out of source builds.
CHECK_OUT_OF_SOURCE()

# To be able to specify a different deployment target on Mac OS X :
# export MACOSX_DEPLOYMENT_TARGET=10.6

IF(CMAKE_VERSION VERSION_GREATER "2.8.10")
  STRING(TIMESTAMP CURRENT_YEAR "%Y")
ELSE()
  SET(CURRENT_YEAR "2016")
ENDIF()

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(NLMisc CXX C)
SET(NL_VERSION_MAJOR 1)
SET(NL_VERSION_MINOR 0)
SET(NL_VERSION_PATCH 0)
SET(YEAR "2004-${CURRENT_YEAR}")
SET(AUTHOR "Winch Gate and The Ryzom Core Community")
SET(NL_VERSION 1.0.0)

#-----------------------------------------------------------------------------
# Redirect output files
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# DLL should be in the same directory as EXE under Windows
IF(WIN32)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
ELSE()
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
ENDIF()

IF(WIN32)
  IF(WITH_MFC)
    FIND_PACKAGE(MFC QUIET)
  ENDIF()
ENDIF()
 
#-----------------------------------------------------------------------------
# Set default config options
#
NL_SETUP_DEFAULT_OPTIONS()

NL_SETUP_BUILD()
NL_SETUP_BUILD_FLAGS()

NL_SETUP_PREFIX_PATHS()
RYZOM_SETUP_PREFIX_PATHS()

#-----------------------------------------------------------------------------
#Platform specifics

SETUP_EXTERNAL()

IF(WIN32)
  SET(WINSOCK2_LIB ws2_32.lib)

  IF(WITH_MFC)
    FIND_PACKAGE(CustomMFC REQUIRED)
  ENDIF()
ENDIF()

IF(WITH_STATIC)
  # libxml2 could need winsock2 library
  IF(WINSOCK2_LIB)
    SET(LIBXML2_LIBRARIES ${LIBXML2_LIBRARIES} ${WINSOCK2_LIB})
  ENDIF()

  IF(UNIX)
    # under Linux and OS X, recent libxml2 versions are linked against liblzma
    FIND_PACKAGE(LibLZMA)
    IF(LIBLZMA_LIBRARIES)
      SET(LIBXML2_LIBRARIES ${LIBXML2_LIBRARIES} ${LIBLZMA_LIBRARIES})
    ENDIF()
  ENDIF()
ENDIF()

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/PCHSupport.cmake)

IF(FINAL_VERSION)
  ADD_DEFINITIONS(-DFINAL_VERSION=1)
ENDIF()

IF(WITH_SSE2)
  ADD_DEFINITIONS(-DNL_HAS_SSE2)
  IF(WITH_SSE3)
    ADD_DEFINITIONS(-DNL_HAS_SSE3)
  ENDIF()
ENDIF()

IF(APPLE)
  FIND_LIBRARY(CARBON_FRAMEWORK Carbon)
  FIND_LIBRARY(FOUNDATION_FRAMEWORK Foundation)
  FIND_LIBRARY(SECURITY_FRAMEWORK Security)

  IF(APPLE_CERTIFICATE)
    # Find codesign_allocate

    # Xcode 7.0 and later versions
    SET(CODESIGN_ALLOCATE ${OSX_DEVELOPER_ROOT}/Toolchains/XcodeDefault.xctoolchain/usr/bin/codesign_allocate)

    IF(NOT EXISTS "${CODESIGN_ALLOCATE}")
      # Xcode 6.4 and previous versions
      SET(CODESIGN_ALLOCATE ${CMAKE_OSX_SYSROOT}/usr/bin/codesign_allocate)
    ENDIF()

    IF(NOT EXISTS "${CODESIGN_ALLOCATE}")
      # System path
      SET(CODESIGN_ALLOCATE /usr/bin/codesign_allocate)
    ENDIF()

    IF(NOT EXISTS "${CODESIGN_ALLOCATE}")
      MESSAGE(WARNING "Unable to find codesign_allocate in standard directories")
      SET(CODESIGN_ALLOCATE)
    ENDIF()
  ENDIF()
ENDIF()

INCLUDE(FindHelpers)

IF(WITH_ASSIMP)
  FIND_PACKAGE(assimp REQUIRED)
ENDIF()

IF(WITH_NEL)
  IF(WITH_NEL_TESTS)
    FIND_PACKAGE(CppTest)
  ENDIF()

  IF(WITH_GUI)
    FIND_PACKAGE(Luabind REQUIRED)
    FIND_PACKAGE(CURL REQUIRED)

    IF((WIN32 OR CURL_LIBRARIES MATCHES "\\.a") AND WITH_STATIC_CURL)
      SET(CURL_STATIC ON)
    ELSE()
      SET(CURL_STATIC OFF)
    ENDIF()

    IF(CURL_STATIC)
      SET(CURL_DEFINITIONS -DCURL_STATICLIB)

      LIST(APPEND CURL_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR})
      LIST(APPEND CURL_LIBRARIES ${OPENSSL_LIBRARIES})

      IF(WIN32)
        LIST(APPEND CURL_LIBRARIES Crypt32 Cryptui)
      ELSE()
        # CURL depends on libidn
        FIND_LIBRARY(IDN_LIBRARY idn)
        IF(IDN_LIBRARY)
          LIST(APPEND CURL_LIBRARIES ${IDN_LIBRARY})
        ENDIF()

        # CURL Macports version can depend on libidn, libidn2, libintl, libpsl and libiconv too
        IF(APPLE)
          FIND_LIBRARY(INTL_LIBRARY intl)
          IF(INTL_LIBRARY)
            LIST(APPEND CURL_LIBRARIES ${INTL_LIBRARY})
          ENDIF()

          FIND_LIBRARY(IDN2_LIBRARY idn2)
          IF(IDN2_LIBRARY)
            LIST(APPEND CURL_LIBRARIES ${IDN2_LIBRARY})
          ENDIF()

          FIND_LIBRARY(PSL_LIBRARY psl)
          IF(PSL_LIBRARY)
            LIST(APPEND CURL_LIBRARIES ${PSL_LIBRARY})
          ENDIF()

          FIND_LIBRARY(UNISTRING_LIBRARY unistring)
          IF(UNISTRING_LIBRARY)
            LIST(APPEND CURL_LIBRARIES ${UNISTRING_LIBRARY})
          ENDIF()

          LIST(APPEND CURL_LIBRARIES ${SECURITY_FRAMEWORK})
        ENDIF()
      ENDIF()
    ENDIF()
  ENDIF()

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/misc/include)
  ADD_SUBDIRECTORY(misc)
  ADD_SUBDIRECTORY(test)

  # Aliases for targets
  SET(NELMISC_LIBRARIES nelmisc)
ELSE()
#  FIND_PACKAGE(NeL COMPONENTS 3d misc pacs sound nimp REQUIRED)
  FIND_PACKAGE(NeL REQUIRED)
  INCLUDE_DIRECTORIES(${NEL_INCLUDE_DIRS})
ENDIF()


IF(WITH_TOOLS)
  ADD_SUBDIRECTORY(tool)
ENDIF()

IF(WITH_NEL_TESTS)
  ENABLE_TESTING()
  ADD_TEST(nel_unit_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/nel_unit_test --html)
  IF(BUILD_DASHBOARD)
    INCLUDE(Dart)
    SET(SVNCOMMAND svn)
    SET(SVNSOURCEDIR http://dev.ryzom.com/svn/trunk/nel)
    SET(GENERATELOGS svn2cl)
  ENDIF()
ENDIF()


