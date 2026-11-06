#---------------------------------------#
#      3DRENDERER MODULE CMAKE        #
#---------------------------------------#
set(3DRENDERER_TARGET_NAME 3DRENDERER)

#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE 3DRENDERER_SOURCE "${K_SOURCE_PATH}/3DRenderer/*.cpp")

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
ENGINE_ADD_MODULE(${3DRENDERER_TARGET_NAME} ${3DRENDERER_SOURCE})

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${3DRENDERER_TARGET_NAME} PUBLIC "${K_INCLUDE_PATH}/3DRenderer")

target_compile_definitions(${3DRENDERER_TARGET_NAME} PUBLIC 3DRENDERER)

target_link_libraries(${3DRENDERER_TARGET_NAME} PRIVATE CORE COREOBJECT spdlog::spdlog KAlloc)