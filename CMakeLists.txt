cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(ARCH "x86_64")
set(CMAKE_CXX_STANDARD 20)

if(WIN32 OR WIN64)
	add_definitions(-DWINDOWS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -Wall -Wextra -Wpedantic -Wformat-security -Wno-unused")
	if(CMAKE_BUILD_TYPE MATCHES Release)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=2 -mmitigate-rop -fPIE")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows -Wl,--high-entropy-va -Wl,--dynamicbase -Wl,--nxcompat -s")
	elseif(CMAKE_BUILD_TYPE MATCHES Debug)
		message(STATUS "Hardening disabled for debug build")
	endif()
elseif(UNIX)
	message(STATUS "Linux is not currently supported")
	add_definitions(-DLINUX)
elseif(APPLE)
	message(STATUS "OSX is not supported")
	add_definitions(-DOSX)
endif()

if(CMAKE_BUILD_TYPE MATCHES Release)
	set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")
	add_definitions(-DRELEASE)
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
	set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0")
	add_definitions(-DDEBUG)
endif()

add_subdirectory(libcommons)
include_directories("libcommons/src/")

set(LIBS
		commons
		"${CMAKE_CURRENT_SOURCE_DIR}/libs/libSDL2.dll.a"
		"${CMAKE_CURRENT_SOURCE_DIR}/libs/libvorbis.a"
		"${CMAKE_CURRENT_SOURCE_DIR}/libs/libogg.a"
		"${CMAKE_CURRENT_SOURCE_DIR}/libs/libpng.a"
		"${CMAKE_CURRENT_SOURCE_DIR}/libs/libz.a"
		"${CMAKE_CURRENT_SOURCE_DIR}/libs/libzstd.a")

set(SRC
		src/glad.c
		
		src/api/render/framebuffer.cc
		src/api/render/framebuffer.hh
		src/api/render/shader.cc src/api/render/shader.hh
		src/api/render/mesh.cc src/api/render/mesh.hh
		src/api/assets/models.cc src/api/assets/models.hh
		src/api/assets/asa.cc src/api/assets/asa.hh
		src/api/assets/camera.cc src/api/assets/camera.hh
		src/api/assets/audio.cc src/api/assets/audio.hh
		src/api/render/renderPass.hh
		src/api/render/renderList.hh
		src/api/render/atlas.cc src/api/render/atlas.hh
		src/api/render/texture.cc src/api/render/texture.hh
		src/api/assets/pngw.cc src/api/assets/pngw.hh
		
		src/main.cc
		src/loft.cc src/loft.hh
		src/def.hh
		src/events.cc src/events.hh
		src/input.cc src/input.hh
		src/global.cc src/global.hh
		src/bsp.hh
		src/assets.cc src/assets.hh
		src/renderer.cc src/renderer.hh
		src/color.cc src/color.hh
		src/postStack.cc src/postStack.hh
		src/object.cc src/object.hh
		src/components.cc src/components.hh
		src/util.cc src/util.hh src/world.cc src/world.hh)

project(LoftEngine)
include_directories(include)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${LIBS})
