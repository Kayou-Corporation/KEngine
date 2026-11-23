#---------------------------------------#
#         KASCADE MODULE CMAKE          #
#---------------------------------------#
set(KASCADE_TARGET_NAME KASCADE)

#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE KASCADE_SOURCE "${K_SOURCE_PATH}/Kascade/*.cpp")

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
ENGINE_ADD_MODULE(${KASCADE_TARGET_NAME} ${KASCADE_SOURCE})

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${KASCADE_TARGET_NAME} PUBLIC "${K_INCLUDE_PATH}/Kascade")

target_link_libraries(${KASCADE_TARGET_NAME} PUBLIC glm::glm assimp stb KUtils KThreads KAlloc)
target_link_libraries(${KASCADE_TARGET_NAME} PUBLIC slang::slang WINDOW CORE)
target_link_libraries(${KASCADE_TARGET_NAME} PRIVATE spdlog::spdlog)