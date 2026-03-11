#---------------------------------------#
#                SOURCES                #
#---------------------------------------#
file(GLOB_RECURSE CORE_SOURCES "Source/*.cpp")

# Define the lib name for easy modification
set(CORE_TARGET_NAME Core)

#---------------------------------------#
#            BUILD MODE                 #
#---------------------------------------#
add_library(${CORE_TARGET_NAME} STATIC ${CORE_SOURCES})

#---------------------------------------#
#                INCLUDE                #
#---------------------------------------#
target_include_directories(${CORE_TARGET_NAME} PUBLIC "Include" "Libs/Glm" "Libs/Stbimage" "Libs")



target_link_libraries(${CORE_TARGET_NAME} PUBLIC glm::glm SDL3::SDL3 spdlog::spdlog assimp)