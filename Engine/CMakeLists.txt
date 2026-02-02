# --------------------------------------- #
#              FIND PACKAGES              #
# --------------------------------------- #
add_subdirectory(Libs)

find_package(Vulkan 1.4 REQUIRED)



#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE K_ENGINE_SOURCES "Code/*.cpp")

# Define the lib name for easy modification
set(K_TARGET_NAME KEngine)



#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
add_library(${K_TARGET_NAME} STATIC ${K_ENGINE_SOURCES})



#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${K_TARGET_NAME} PUBLIC "Code/Include" "Libs/Glm" "Libs/Stbimage" "Libs")



#---------------------------------------#
#                COMPILE MODE           #
#---------------------------------------#
target_compile_features(${K_TARGET_NAME} PUBLIC cxx_std_23)

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    target_compile_options(${K_TARGET_NAME} PUBLIC /W4 /WX)
else()
    target_compile_options(${K_TARGET_NAME} PUBLIC -Wall -Wextra -Wpedantic)
endif()

# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Build position independent code.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Disable C and C++ compiler extensions.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_link_libraries(${K_TARGET_NAME} PUBLIC Vulkan::Vulkan glm::glm SDL3::SDL3 spdlog::spdlog GPUOpen::VulkanMemoryAllocator assimp)



#---------------------------------------#
#        EXPORTS / IMPORTS FOR DLL      #
#---------------------------------------#
# Disable lib prefix
set_target_properties(${K_TARGET_NAME} PROPERTIES PREFIX "")

# Set dll name
set_target_properties(${K_TARGET_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")



#---------------------------------------#
#           COPY ASSETS FOLDER          #
#---------------------------------------#
# Copy assets after target is built
add_custom_command(
        TARGET ${K_TARGET_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        "${K_OUTPUT_DIR}/Engine/Assets"
        COMMENT "Copying Assets to output directory after build"
)



# Ensure Assets are also present in the general binary dir (for IDEs like Visual Studio)
add_custom_target(CopyAssets ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/Engine/Assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        "${CMAKE_BINARY_DIR}/Engine/Assets"
        COMMENT "Forcefully copying Assets to build directory"
)



# Make sure CopyAssets is executed before building the target
add_dependencies(${K_TARGET_NAME} CopyAssets)