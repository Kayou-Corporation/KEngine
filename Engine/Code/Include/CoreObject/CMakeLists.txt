#---------------------------------------#
#       COREOBJECT MODULE CMAKE         #
#---------------------------------------#
set(COREOBJECT_TARGET_NAME COREOBJECT)

#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE COREOBJECT_SOURCE "${K_SOURCE_PATH}/CoreObject/*.cpp")

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
ENGINE_ADD_MODULE(${COREOBJECT_TARGET_NAME} ${COREOBJECT_SOURCE})

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${COREOBJECT_TARGET_NAME} PUBLIC "${K_INCLUDE_PATH}/CoreObject")

target_compile_definitions(${COREOBJECT_TARGET_NAME} PUBLIC COREOBJECT)

target_link_libraries(${COREOBJECT_TARGET_NAME} PRIVATE CORE ASSETMANAGER spdlog::spdlog)