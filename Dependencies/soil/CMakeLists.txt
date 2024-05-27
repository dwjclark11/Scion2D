add_library(soil
  "src/image_dxt.c"
  "src/image_helper.c"
  "src/soil.c"
  "src/stb_image_aug.c")

target_include_directories(
  soil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)