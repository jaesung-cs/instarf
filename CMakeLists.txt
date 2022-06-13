cmake_minimum_required(VERSION 3.18)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(irf)

find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Vulkan REQUIRED)

add_subdirectory(dependencies/VulkanMemoryAllocator)

# irf shader
file(GLOB_RECURSE IRF_SHADER_SOURCE_VERT_FILES ${PROJECT_SOURCE_DIR}/assets/shaders/*.vert)
file(GLOB_RECURSE IRF_SHADER_SOURCE_FRAG_FILES ${PROJECT_SOURCE_DIR}/assets/shaders/*.frag)
file(GLOB_RECURSE IRF_SHADER_SOURCE_COMP_FILES ${PROJECT_SOURCE_DIR}/assets/shaders/*.comp)
set(IRF_SHADER_SOURCE_FILES
  ${IRF_SHADER_SOURCE_VERT_FILES}
  ${IRF_SHADER_SOURCE_FRAG_FILES}
  ${IRF_SHADER_SOURCE_COMP_FILES}
)

# shader compile commands
foreach(SHADER_FILE ${IRF_SHADER_SOURCE_FILES})
  set(SHADER_OUTPUT_FILE ${SHADER_FILE}.spv)
  list(APPEND IRF_SHADER_OUTPUT_FILES ${SHADER_OUTPUT_FILE})

  add_custom_command(
    OUTPUT ${SHADER_OUTPUT_FILE}
    COMMAND glslc.exe -o ${SHADER_OUTPUT_FILE} --target-env=vulkan1.1 ${SHADER_FILE}
    DEPENDS ${SHADER_FILE}
    VERBATIM
  )
endforeach()

add_custom_target(irf_shader DEPENDS ${IRF_SHADER_OUTPUT_FILES})

# irf_gpu
file(GLOB_RECURSE IRF_GPU_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/instarf/gpu/*.cc)
file(GLOB_RECURSE IRF_GPU_INCLUDE_FILES
  ${PROJECT_SOURCE_DIR}/include/instarf/gpu/*.h
  ${PROJECT_SOURCE_DIR}/src/instarf/gpu/*.h
)

add_library(irf_gpu
  ${IRF_GPU_SOURCE_FILES}
  ${IRF_GPU_INCLUDE_FILES}
)

target_include_directories(irf_gpu
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(irf_gpu
  Vulkan::Vulkan
  glm::glm
  VulkanMemoryAllocator
)

# irf
file(GLOB IRF_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/instarf/*.cc)
file(GLOB IRF_INCLUDE_FILES
  ${PROJECT_SOURCE_DIR}/include/instarf/*.h
  ${PROJECT_SOURCE_DIR}/include/instarf/shader/*.h
  ${PROJECT_SOURCE_DIR}/src/instarf/*.h
)

add_executable(irf
  ${IRF_SOURCE_FILES}
  ${IRF_INCLUDE_FILES}
  src/main.cc
)

add_dependencies(irf irf_shader)

target_include_directories(irf PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(irf
  irf_gpu
  glfw
  imgui::imgui
  opencv_core opencv_imgcodecs
)

# test
file(GLOB TEST_IRF_SOURCE_FILES ${PROJECT_SOURCE_DIR}/test/*.cc)

add_executable(test_irf
  ${TEST_IRF_SOURCE_FILES}
)

add_dependencies(test_irf irf_shader)

target_link_libraries(test_irf
  irf_gpu
)

# copy assets
file(GLOB_RECURSE ASSET_FILES
  RELATIVE ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/assets/*)

foreach(ASSET_FILE ${ASSET_FILES})
  set(ASSET_TARGET_FILE ${CMAKE_BINARY_DIR}/${ASSET_FILE})
  list(APPEND ASSET_TARGET_FILES ${ASSET_TARGET_FILE})

  add_custom_command(
    OUTPUT ${ASSET_TARGET_FILE}
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/${ASSET_FILE} ${ASSET_TARGET_FILE}
    DEPENDS ${PROJECT_SOURCE_DIR}/${ASSET_FILE}
    VERBATIM
  )
endforeach()

add_custom_target(copy_assets DEPENDS ${ASSET_TARGET_FILES})

add_dependencies(irf copy_assets)
