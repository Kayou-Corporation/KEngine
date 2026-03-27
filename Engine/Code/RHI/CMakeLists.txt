#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE RHI_SOURCES "Source/*.cpp")

# Define the lib name for easy modification
set(RHI_TARGET_NAME RHI)

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
ENGINE_ADD_MODULE(${RHI_TARGET_NAME} ${RHI_SOURCES})

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${RHI_TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include")



target_link_libraries(${RHI_TARGET_NAME} PUBLIC GPUOpen::VulkanMemoryAllocator slang Core)