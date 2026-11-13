#---------------------------------------#
#      CORERUNTIME MODULE CMAKE         #
#---------------------------------------#
set(CORERUNTIME_TARGET_NAME CORERUNTIME)

#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE CORERUNTIME_SOURCE "${K_SOURCE_PATH}/CoreRuntime/*.cpp")

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
ENGINE_ADD_MODULE(${CORERUNTIME_TARGET_NAME} ${CORERUNTIME_SOURCE})

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${CORERUNTIME_TARGET_NAME} PUBLIC "${K_INCLUDE_PATH}/CoreRuntime")

target_compile_definitions(${CORERUNTIME_TARGET_NAME} PUBLIC CORERUNTIME)

target_link_libraries(${CORERUNTIME_TARGET_NAME} PRIVATE CORE COREOBJECT)
target_link_libraries(${CORERUNTIME_TARGET_NAME} PRIVATE glm::glm spdlog::spdlog)
