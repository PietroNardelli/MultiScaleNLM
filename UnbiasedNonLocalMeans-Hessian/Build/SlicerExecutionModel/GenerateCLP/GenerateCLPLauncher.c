#if defined(_WIN32)
#  if defined(CMAKE_INTDIR)
#   define itksys_SHARED_FORWARD_CONFIG_NAME CMAKE_INTDIR
#    define CONFIG_DIR_PRE CMAKE_INTDIR "/"
#    define CONFIG_DIR_POST "/" CMAKE_INTDIR
#  else
#    define CONFIG_DIR_PRE ""
#    define CONFIG_DIR_POST ""
#  endif
#endif

#define itksys_SHARED_FORWARD_DIR_BUILD "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build"
#define itksys_SHARED_FORWARD_PATH_BUILD "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build","/net/th914_nas.bwh.harvard.edu/mnt/array1/share/Software/Slicer4.3Build-Relase/ITKv4-build/bin"
#define itksys_SHARED_FORWARD_PATH_INSTALL "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/SlicerExecutionModel/GenerateCLP","/net/th914_nas.bwh.harvard.edu/mnt/array1/share/Software/Slicer4.3Build-Relase/ITKv4-build","/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/SlicerExecutionModel/ModuleDescriptionParser"
#if defined(_WIN32)
#  define itksys_SHARED_FORWARD_EXE_BUILD CONFIG_DIR_PRE "GenerateCLP"
#else
#  define itksys_SHARED_FORWARD_EXE_BUILD "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/GenerateCLP"
#endif
#define itksys_SHARED_FORWARD_EXE_INSTALL "./GenerateCLP"
#define itksys_SHARED_FORWARD_OPTION_PRINT "--print"
#define itksys_SHARED_FORWARD_OPTION_LDD "--ldd"

#include <itksys/SharedForward.h>

int main(int argc, char *argv[])
{
  return itksys_shared_forward_to_real(argc, argv);
}
