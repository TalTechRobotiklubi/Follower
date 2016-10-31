if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  add_definitions(-DFHD_OMP)
endif()

add_library(fhd
  fhd.cpp
  fhd_block_allocator.cpp
  fhd_candidate.cpp
  fhd_classifier.cpp
  fhd_hash.cpp
  fhd_image.cpp
  fhd_kinect.cpp
  fhd_math.cpp
  fhd_perf.cpp
  fhd_sampler.cpp
  fhd_segmentation.cpp

  pcg/pcg_basic.c
)

set (FHD_HEADERS
  fhd.h
  fhd_candidate.h
  fhd_config.h
  fhd_image.h
  fhd_math.h
  fhd_perf.h
  fhd_sampler.h
)
