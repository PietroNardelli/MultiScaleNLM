# Install script for directory: /home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/ModuleDescriptionParser

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ModuleDescriptionParser" TYPE FILE FILES
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/ModuleDescriptionParser/BatchMakeUtilities.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/ModuleDescriptionParser/BinaryFileDescriptor.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/ModuleDescriptionParser/ModuleDescription.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/ModuleDescriptionParser/ModuleDescriptionParser.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/ModuleDescriptionParser/ModuleDescriptionParserWin32Header.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/ModuleDescriptionParser/ModuleDescriptionUtilities.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/ModuleDescriptionParser/ModuleFactory.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/ModuleDescriptionParser/ModuleLogo.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/ModuleDescriptionParser/ModuleParameter.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/ModuleDescriptionParser/ModuleParameterGroup.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/ModuleDescriptionParser/ModuleProcessInformation.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/SlicerExecutionModel/ModuleDescriptionParser/ModuleDescriptionParserConfigure.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "RuntimeLibraries")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/ModuleDescriptionParser/libModuleDescriptionParser.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/ModuleDescriptionParser/libModuleDescriptionParser.so")
    FILE(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/ModuleDescriptionParser/libModuleDescriptionParser.so"
         RPATH "")
  ENDIF()
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ModuleDescriptionParser" TYPE SHARED_LIBRARY FILES "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/libModuleDescriptionParser.so")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/ModuleDescriptionParser/libModuleDescriptionParser.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/ModuleDescriptionParser/libModuleDescriptionParser.so")
    FILE(RPATH_REMOVE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/ModuleDescriptionParser/libModuleDescriptionParser.so")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/ModuleDescriptionParser/libModuleDescriptionParser.so")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "RuntimeLibraries")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ModuleDescriptionParser" TYPE FILE FILES
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/SlicerExecutionModel/ModuleDescriptionParser/UseModuleDescriptionParser.cmake"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/SlicerExecutionModel/ModuleDescriptionParser/install/ModuleDescriptionParserConfig.cmake"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")

