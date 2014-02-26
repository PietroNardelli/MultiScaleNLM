# ModuleDescriptionParser could be installed anywhere, so set all paths based 
# on where this file was found (which should be the lib/ModuleDescriptionParser
# directory of the installation)
get_filename_component(ModuleDescriptionParser_CONFIG_DIR 
  "${CMAKE_CURRENT_LIST_FILE}" PATH
  )

# 
set(ModuleDescriptionParser_INCLUDE_DIRS 
  "${ModuleDescriptionParser_CONFIG_DIR}/../../include/ModuleDescriptionParser"
  )
set(ModuleDescriptionParser_LIBRARY_DIRS 
  "${ModuleDescriptionParser_CONFIG_DIR}"
  )
set(ModuleDescriptionParser_USE_FILE 
  "${ModuleDescriptionParser_CONFIG_DIR}/UseModuleDescriptionParser.cmake"
  )

set(ITK_DIR "/net/th914_nas.bwh.harvard.edu/mnt/array1/share/Software/Slicer4.3Build-Relase/ITKv4-build")
