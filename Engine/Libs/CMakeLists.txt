# ----------- GLM ----------- #
set(DGLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(Glm)




# ----------- SDL3 ----------- #
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)

add_subdirectory(SDL)




# ----------- SPDLog ----------- #
set(SPDLOG_FMT_EXTERNAL OFF)
set(SPDLOG_BUILD_SHARED OFF)
set(SPDLOG_BUILD_EXAMPLE OFF)
set(SPDLOG_BUILD_TESTS OFF)
set(SPDLOG_BUILD_BENCH OFF)

add_subdirectory(Spdlog)