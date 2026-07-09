# ----------- GLM ----------- #
set(DGLM_BUILD_TESTS OFF CACHE BOOL "Disable GLM tests")

add_subdirectory(Glm)



# ----------- SDL3 ----------- #
# SDL Static build with no tests
set(SDL_SHARED OFF CACHE BOOL "Disable SDL shared library")
set(SDL_STATIC ON CACHE BOOL "Enable SDL static library")
set(SDL_TEST OFF CACHE BOOL "Disable SDL tests")

add_subdirectory(SDL)



# ----------- SPDLog ----------- #
# SPDLog static build with no examples, tests & benchmarks
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "Do not use external fmt library")
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "Build SPDLog as static library")
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "Do not build examples")
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "Do not build tests")
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "Do not build benchmarks")

add_subdirectory(Spdlog)



# ----------- VulkanMemoryAllocator ----------- #
set(VMA_STATIC_VULKAN_FUNCTIONS ON CACHE BOOL "Link vulkan statically")

add_subdirectory(VulkanMemoryAllocator)



# ----------- Assimp ----------- #
# Only enable FBX & GLTF Support
set(ASSIMP_BUILD_SHARED_LIBS OFF CACHE BOOL "Build Assimp statically")
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "Disable all default importers")
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "Disable all default exporters")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Disable Assimp tools")
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "Disable Assimp viewer")
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "Disable documentation")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable tests")
set(ASSIMP_INSTALL OFF CACHE BOOL "Do not install Assimp")

set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "Enable OBJ importer")
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "Enable GLTF importer")
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "Enable FBX importer")
set(ASSIMP_BUILD_ASSBIN_EXPORTER OFF CACHE BOOL "Disable ASSBIN exporter")
set(ASSIMP_BUILD_ASSXML_EXPORTER OFF CACHE BOOL "Disable ASSXML exporter")

add_subdirectory(Assimp)

if(TARGET assimp)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(assimp PRIVATE -Wno-error=maybe-uninitialized -Wno-maybe-uninitialized -Wno-error=uninitialized -Wno-uninitialized)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(assimp PRIVATE -Wno-error=uninitialized -Wno-uninitialized)
    endif()
endif()

# ----------- STB ----------- #
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Stbimage")

# ----------- KUtils ----------- #
add_subdirectory(KUtils)

