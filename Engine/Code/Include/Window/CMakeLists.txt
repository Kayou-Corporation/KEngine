#---------------------------------------#
#         WINDOW MODULE CMAKE           #
#---------------------------------------#
set(WINDOW_TARGET_NAME WINDOW)

#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE WINDOW_SOURCE "${K_SOURCE_PATH}/Window/*.cpp")

message(STATUS "L'enfant a acc�s au chemin : ${K_SOURCE_PATH}")

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
ENGINE_ADD_MODULE(${WINDOW_TARGET_NAME} ${WINDOW_SOURCE})

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${WINDOW_TARGET_NAME} PUBLIC "${K_INCLUDE_PATH}/Window")

target_link_libraries(${WINDOW_TARGET_NAME} PUBLIC SDL3::SDL3 CORE)
target_link_libraries(${WINDOW_TARGET_NAME} PRIVATE spdlog::spdlog)