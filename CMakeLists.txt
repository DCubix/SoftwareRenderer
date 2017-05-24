cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(pix3d)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")

find_package(SDL REQUIRED)
find_package(SDL_gfx REQUIRED)

include_directories(
	${SDL_INCLUDE_DIR}
	${SDLGFX_INCLUDE_DIR}
)

file(GLOB SRC "src/*.c" "src/*.h")

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME}
	${SDL_LIBRARY}
	${SDLGFX_LIBRARY}
	m
)
