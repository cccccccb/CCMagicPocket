set(SOURCES fast_gaussian_blur_template.h)

add_library(fast_blur STATIC ${SOURCES})
target_include_directories(fast_blur PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
