cmake_minimum_required(VERSION 2.8)
project( UnbiasedNonLocalMeans )
# 
# --------------------------------------------------------------------------
# Special build configs (to handle linking on windows)
#CONFIGURE_FILE(
#  vtkRealignConfigure.h.in
#  Applications/CLI/Realign/vtkRealignConfigure.h
#)


################################################################################
set(STANDALONE_CMAKE_HELPER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/BuildScripts)
include(${STANDALONE_CMAKE_HELPER_DIR}/CMakeBuildMacros.cmake)
include(${STANDALONE_CMAKE_HELPER_DIR}/PreventInSourceBuilds.cmake)
include(${STANDALONE_CMAKE_HELPER_DIR}/CMakeSTANDALONE3BuildMacros.cmake)

###
SETIFEMPTY(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
SETIFEMPTY(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
SETIFEMPTY(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
SETIFEMPTY(CMAKE_BUNDLE_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
###

CHECKIFSLICER3BUILD()  ## Call the convenience macro

if (Slicer_SOURCE_DIR)
    if(Slicer_BINARY_DIR)
    else(Slicer_BINARY_DIR)
        # Slicer_BINARY_DIR only exists for Slicer 4. This flag is only used for Slicer3 builds:
        SET_SOURCE_FILES_PROPERTIES(${PROJECT_NAME}.cxx COMPILE_FLAGS -DTHISISASLICERBUILD=1)
    endif(Slicer_BINARY_DIR)
else( Slicer_SOURCE_DIR )
    ADD_SUBDIRECTORY(SlicerExecutionModel)
    OPTION(BUILD_TESTING "Build tests for this program" ON)
    if( BUILD_TESTING )
        ENABLE_TESTING()
    endif( BUILD_TESTING )
endif (Slicer_SOURCE_DIR)
################################################################################

OPTION(BUILD_CMPB_VERSIONS "Build the exact same source code used for the CMPB paper" OFF)
if( BUILD_CMPB_VERSIONS )
    ADD_SUBDIRECTORY( cmpb.source.code )
endif( BUILD_CMPB_VERSIONS )


if(NOT ITK_FOUND)
    find_package(ITK REQUIRED)
    include(${ITK_USE_FILE})
endif(NOT ITK_FOUND)

if(ITK_VERSION_MAJOR LESS 4)
    set(ITK_LIBRARIES ITKIO ITKNumerics ITKStatistics ITKAlgorithms ITKBasicFilters)
endif(ITK_VERSION_MAJOR LESS 4)

# For Slicer 4 builds, simply call the standard macro:
if(Slicer_BINARY_DIR) # This variable exits only for Slicer4

    slicerMacroBuildCLI(
       NAME ${PROJECT_NAME}
       LOGO_HEADER ${PROJECT_SOURCE_DIR}/ModuleLogo.h
       TARGET_LIBRARIES ${ITK_LIBRARIES} ModuleDescriptionParser
       LINK_DIRECTORIES ${ModuleDescriptionParser_BINARY_DIR}
       INCLUDE_DIRECTORIES ${SlicerBaseCLI_SOURCE_DIR} ${Slicer_SOURCE_DIR}/Applications/CLI/DiffusionApplications/DiffusionApplicationsCommon
       ${SlicerBaseCLI_BINARY_DIR}
    )
    # This is necessary to tell the main .cxx what kind of build we are using:
    SET_SOURCE_FILES_PROPERTIES(${PROJECT_NAME}.cxx COMPILE_FLAGS -DSLICERV4=1)
    
else(Slicer_BINARY_DIR)
    set(EXTRA_HEADERS
        itkNLMFilter.txx
        LSDerivatives/itkLSDerivatives.txx
    )
    set(CLP UnbiasedNonLocalMeans)
    set ( ${CLP}_SOURCE ${CLP}.cxx )
    CONFIGURESTANDALONEORSLICERPROPERTIES(${CLP} ${CLP}.xml "${${CLP}_SOURCE}" "" "${EXTRA_HEADERS}")
    target_link_libraries( ${CLP} ${ITK_LIBRARIES} )
    
    INSTALL(TARGETS UnbiasedNonLocalMeans DESTINATION bin)
 
endif(Slicer_BINARY_DIR)


if (BUILD_TESTING)
  add_executable(${CLP}_Compare ${CLP}_Compare.cxx)
  target_link_libraries(${CLP}_Compare ${ITK_LIBRARIES} )
  
  set(TEST_DATA_DIR ${PROJECT_SOURCE_DIR}/TestData)
  if (Slicer_SOURCE_DIR)
        
        # This is a Slicer build
        # Depending on the actual version (3 or 4), the calling sequence differs:
        if(Slicer3_BINARY_DIR)
            # This is for Slicer3
            set(OUTPUT_DIR ${Slicer3_BINARY_DIR}/Testing)
            set(Slicer_EXE ${Slicer3_BINARY_DIR}/Slicer3)
            SET(RUNEXEC
                ${Slicer_EXE}
                --launch
                ${CLP}
            )
            SET(COMPEXEC
                ${Slicer_EXE}
                --launch
                ${CLP}_Compare
            )
        endif(Slicer3_BINARY_DIR)
        
        if(Slicer_BINARY_DIR)
            # And this is for Slicer4. It requires calling the CLI with the full path
            set(OUTPUT_DIR ${Slicer_BINARY_DIR}/Testing)
            set(Slicer_EXE ${Slicer_BINARY_DIR}/Slicer)
            SET(RUNEXEC
                ${Slicer_EXE}
                --launch
                "${Slicer_BINARY_DIR}/lib/Slicer-${Slicer_VERSION_MAJOR}.${Slicer_VERSION_MINOR}/cli-modules/${CLP}"
            )
            SET(COMPEXEC
                ${Slicer_EXE}
                --launch
                "${Slicer_BINARY_DIR}/bin/${CLP}_Compare"
            )
        endif(Slicer_BINARY_DIR)
  
  else( Slicer_SOURCE_DIR )
    set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/Testing)
    SET( RUNEXEC ${CMAKE_BINARY_DIR}/${CLP} )
    SET( COMPEXEC ${CMAKE_BINARY_DIR}/${CLP}_Compare )
  endif (Slicer_SOURCE_DIR)
 
  # This runs the module with testing parameters:
  add_test(${PROJECT_NAME}_SampleTest
    ${RUNEXEC}
    --sigma 65
    --rs 3,3,3
    --rc 1,1,1
    --hp 1.0
    --ps 2.0
    ${TEST_DATA_DIR}/t2crop.nii.gz
    ${OUTPUT_DIR}/t2filteredSample.nii.gz
  )
 
  # This runs a program that checks the output and compares to the one expected:
  add_test(${PROJECT_NAME}_SampleCompare
    ${COMPEXEC}
    ${OUTPUT_DIR}/t2filteredSample.nii.gz
    ${TEST_DATA_DIR}/t2filtered.nii.gz
  )
 
endif (BUILD_TESTING)
