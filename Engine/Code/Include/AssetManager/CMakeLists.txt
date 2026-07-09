#---------------------------------------#
#      ASSETMANAGER MODULE CMAKE        #
#---------------------------------------#
set(ASSETMANAGER_TARGET_NAME ASSETMANAGER)

#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE ASSETMANAGER_SOURCE "${K_SOURCE_PATH}/AssetManager/*.cpp")

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
ENGINE_ADD_MODULE(${ASSETMANAGER_TARGET_NAME} ${ASSETMANAGER_SOURCE})

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${ASSETMANAGER_TARGET_NAME} PUBLIC "${K_INCLUDE_PATH}/AssetManager")

target_compile_definitions(${ASSETMANAGER_TARGET_NAME} PUBLIC ASSETMANAGER)

target_link_libraries(${ASSETMANAGER_TARGET_NAME} PUBLIC assimp stb KThreads CORE)