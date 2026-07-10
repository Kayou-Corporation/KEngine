# --------------------------------------- #
#              ENGINE CMAKE               #
# --------------------------------------- #

# --------------------------------------- #
#              UTILS                      #
# --------------------------------------- #
set(K_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Code/Include")
set(K_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Code/Source")

set(K_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Code/Source/Engine.cpp")

# --------------------------------------- #
#              FIND PACKAGES              #
# --------------------------------------- #
add_subdirectory(Libs)

find_package(Vulkan 1.4)
if (Vulkan_FOUND)
    set(VULKAN_ENABLE ON CACHE BOOL "Enable Vulkan renderer")
else()
    set(VULKAN_ENABLE OFF CACHE BOOL "Enable Vulkan renderer")
endif()

find_package(slang REQUIRED)

#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
# Define the lib name for easy modification
set(K_TARGET_NAME KEngine)

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
add_library(${K_TARGET_NAME} SHARED ${K_ENGINE_PATH})

#---------------------------------------#
#                COMPILE MODE           #
#---------------------------------------#
set(ENGINE_COMPILE_OPTIONS
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

set(ENGINE_COMPILE_FEATURES cxx_std_23)

function(engine_add_module MODULE_NAME)
    add_library(${MODULE_NAME} STATIC ${ARGN})
    target_compile_features(${MODULE_NAME} PUBLIC ${ENGINE_COMPILE_FEATURES})
    target_compile_options(${MODULE_NAME} PUBLIC ${ENGINE_COMPILE_OPTIONS})
    target_include_directories(${MODULE_NAME} PUBLIC Include)
    target_compile_definitions(${MODULE_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(${MODULE_NAME} PRIVATE _CRT_SECURE_NO_DEPRECATE)
    target_compile_definitions(${MODULE_NAME} PRIVATE EXPORTS)

endfunction()

# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Build position independent code.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Disable C and C++ compiler extensions.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#---------------------------------------#
#                MODULE LIST            #
#---------------------------------------#
add_subdirectory(${K_INCLUDE_PATH}/Core)
target_link_libraries(${K_TARGET_NAME} PUBLIC CORE)

add_subdirectory(${K_INCLUDE_PATH}/Window)
target_link_libraries(${K_TARGET_NAME} PUBLIC WINDOW)

add_subdirectory(${K_INCLUDE_PATH}/CoreRuntime)
target_link_libraries(${K_TARGET_NAME} PUBLIC CORERUNTIME)

add_subdirectory(${K_INCLUDE_PATH}/AssetManager)
target_link_libraries(${K_TARGET_NAME} PUBLIC ASSETMANAGER)

#target_include_directories(${K_TARGET_NAME} "Libs/Stbimage")

#if (LINK_RHI)
#    target_link_libraries(${K_TARGET_NAME} PUBLIC RHI)
#endif()


#---------------------------------------#
#        EXPORTS / IMPORTS FOR DLL      #
#---------------------------------------#
# Disable lib prefix
set_target_properties(${K_TARGET_NAME} PROPERTIES PREFIX "")

# Set dll name
set_target_properties(${K_TARGET_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")

# Set export macro
target_compile_definitions(${K_TARGET_NAME} PRIVATE EXPORTS)

#---------------------------------------#
#           COPY ASSETS FOLDER          #
#---------------------------------------#
# Copy assets after target is built
add_custom_command(
        TARGET ${K_TARGET_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        "${OUTPUT_DIR_BASE}/Engine/Assets"
        COMMENT "Copying Assets to output directory after build"
)

# Copy slang-compiler.dll to the output directory after target is built
if (WIN32)
    add_custom_command(
            TARGET ${K_TARGET_NAME} 
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/Libs/Slang/build/Release/bin/slang-compiler.dll"
            "${OUTPUT_DIR_BASE}/"
    )
endif()

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