# CMake generated Testfile for 
# Source directory: /home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source
# Build directory: /home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build
# 
# This file includes the relevent testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(UnbiasedNonLocalMeans_SampleTest "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/UnbiasedNonLocalMeans" "--sigma" "65" "--rs" "3,3,3" "--rc" "1,1,1" "--hp" "1.0" "--ps" "2.0" "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/TestData/t2crop.nii.gz" "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/Testing/t2filteredSample.nii.gz")
ADD_TEST(UnbiasedNonLocalMeans_SampleCompare "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/UnbiasedNonLocalMeans_Compare" "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Build/Testing/t2filteredSample.nii.gz" "/home/pietro/projects/UnbiasedNonLocalMeans-Hessian/Source/TestData/t2filtered.nii.gz")
SUBDIRS(SlicerExecutionModel)
