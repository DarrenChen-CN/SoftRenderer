set(IMGUI_PATH ${PROJECT_SOURCE_DIR}/thirdparty/imgui)
set(GLFW_PATH ${PROJECT_SOURCE_DIR}/thirdparty/glfw)
set(GLAD_PATH ${PROJECT_SOURCE_DIR}/thirdparty/glad)
set(EIGEN_PATH ${PROJECT_SOURCE_DIR}/thirdparty/Eigen3)
set(SRC_PATH ${PROJECT_SOURCE_DIR}/src)
set(ASSIMP_PATH ${PROJECT_SOURCE_DIR}/thirdparty/assimp)
set(STB_IMAGE_PATH ${PROJECT_SOURCE_DIR}/thirdparty/stb_image)

include_directories(${IMGUI_PATH})
include_directories(${GLFW_PATH}/include)
include_directories(${ASSIMP_PATH}/include)
include_directories(${EIGEN_PATH}/include/Eigen3)
include_directories(${STB_IMAGE_PATH})
include_directories(core)
include_directories(shader)
include_directories(utils)

link_directories(${ASSIMP_PATH}/lib)

message(${GLAD_PATH}/glad.c)

file(GLOB GLAD_SRC ${GLAD_PATH}/glad.c)
file(GLOB IMGUI_SRC ${IMGUI_PATH}/*.cpp)
file(GLOB_RECURSE SRC ${SRC_PATH}/*.cpp)
file(GLOB ASSIMP_DLL ${ASSIMP_PATH}/lib/*.dll)


link_directories(${GLFW_PATH}/lib)

add_executable(SoftRenderer ${SRC} ${IMGUI_SRC} ${GLAD_SRC})

target_link_libraries(SoftRenderer glfw3.dll ${ASSIMP_DLL})

add_custom_command(
            TARGET SoftRenderer POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSIMP_DLL} "${CMAKE_BINARY_DIR}/src"
            COMMAND_EXPAND_LISTS
    )