# Install script for directory: /home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/tclap" TYPE FILE FILES
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/Arg.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/ArgException.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/ArgOld.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/CmdLine.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/CmdLineInterface.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/CmdLineOutput.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/Constraint.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/DocBookOutput.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/HelpVisitor.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/IgnoreRestVisitor.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/MultiArg.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/MultiArgOld.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/MultiSwitchArg.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/OptionalUnlabeledTracker.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/StdOutput.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/SwitchArg.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/UnlabeledMultiArg.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/UnlabeledValueArg.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/ValueArg.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/ValueArgOld.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/ValuesConstraint.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/VersionVisitor.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/Visitor.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/XMLOutput.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/include/tclap/XorHandler.h"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/SlicerExecutionModel/tclap/COPYING"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/tclap" TYPE FILE FILES
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/SlicerExecutionModel/tclap/UseTCLAP.cmake"
    "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/SlicerExecutionModel/tclap/install/TCLAPConfig.cmake"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")

