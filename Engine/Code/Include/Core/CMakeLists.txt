#---------------------------------------#
#         CORE MODULE CMAKE             #
#---------------------------------------#
set(CORE_TARGET_NAME CORE)

#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE CORE_SOURCE "${K_SOURCE_PATH}/Core/*.cpp")

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
ENGINE_ADD_MODULE(${CORE_TARGET_NAME} ${CORE_SOURCE})

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${CORE_TARGET_NAME} PUBLIC "${K_INCLUDE_PATH}/Core")

message(STATUS "vulkan enabled : ${VULKAN_ENABLE}")

target_compile_definitions(${CORE_TARGET_NAME} PUBLIC VULKAN_ENABLE)
target_link_libraries(${CORE_TARGET_NAME} PUBLIC Vulkan::Vulkan glm::glm assimp stb KUtils KThreads)
target_link_libraries(${CORE_TARGET_NAME} PUBLIC GPUOpen::VulkanMemoryAllocator slang::slang WINDOW)
target_link_libraries(${CORE_TARGET_NAME} PRIVATE spdlog::spdlog)