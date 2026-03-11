#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE RHI_SOURCES "Source/*.cpp")

# Define the lib name for easy modification
set(RHI_TARGET_NAME RHI)

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
add_library(${RHI_TARGET_NAME} STATIC ${RHI_SOURCES})

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${RHI_TARGET_NAME} PUBLIC "Include")



target_link_libraries(${RHI_TARGET_NAME} PRIVATE Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator Core)