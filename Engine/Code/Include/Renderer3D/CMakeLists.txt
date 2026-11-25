#---------------------------------------#
#      3DRENDERER MODULE CMAKE        #
#---------------------------------------#
set(RENDERER3D_TARGET_NAME RENDERER3D)

#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE RENDERER3D_SOURCE "${K_SOURCE_PATH}/Renderer3D/*.cpp")

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
ENGINE_ADD_MODULE(${RENDERER3D_TARGET_NAME} ${RENDERER3D_SOURCE})

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${RENDERER3D_TARGET_NAME} PUBLIC "${K_INCLUDE_PATH}/Renderer3D")

target_compile_definitions(${RENDERER3D_TARGET_NAME} PUBLIC RENDERER3D)

target_link_libraries(${RENDERER3D_TARGET_NAME} PRIVATE CORE COREOBJECT spdlog::spdlog KAlloc)
target_include_directories(${RENDERER3D_TARGET_NAME} PRIVATE "${K_INCLUDE_PATH}/Engine")