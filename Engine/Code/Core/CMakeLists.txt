#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE CORE_SOURCES "Source/*.cpp")

# Define the lib name for easy modification
set(CORE_TARGET_NAME Core)

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
ENGINE_ADD_MODULE(${CORE_TARGET_NAME} ${CORE_SOURCES})

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${CORE_TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include" "Libs/Glm" "Libs/Stbimage" "Libs")

message(STATUS "vulkan enabled : ${VULKAN_ENABLE}")

target_compile_definitions(${CORE_TARGET_NAME} PUBLIC VULKAN_ENABLE)

target_link_libraries(${CORE_TARGET_NAME} PUBLIC Vulkan::Vulkan glm::glm SDL3::SDL3 spdlog::spdlog assimp)