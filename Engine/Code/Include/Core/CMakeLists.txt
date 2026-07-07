#---------------------------------------#
#         CORE MODULE CMAKE             #
#---------------------------------------#
set(CORE_TARGET_NAME CORE)

#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE CORE_SOURCE "${K_SOURCE_PATH}/Core/*.cpp")

message(STATUS "L'enfant a accès au chemin : ${K_SOURCE_PATH}")

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
ENGINE_ADD_MODULE(${CORE_TARGET_NAME} ${CORE_SOURCE})

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${CORE_TARGET_NAME} PUBLIC "${K_INCLUDE_PATH}/Core")

target_link_libraries(${CORE_TARGET_NAME} PUBLIC GPUOpen::VulkanMemoryAllocator slang::slang Core)