cmake_minimum_required (VERSION 2.8.1)

######## Project settings ########
PROJECT(neon-hello)
SET(LICENSE "TBD")

######## Build and include settings ########
include_directories(
	inc
)

link_directories(
	${LINK_DIRECTORIES}
)


file(GLOB SOURCES
	"src/*.cpp"
)

add_executable(
	neon-hello

	${SOURCES}
)

TARGET_LINK_LIBRARIES(
	neon-hello
)

######## Install targets ########
INSTALL(TARGETS neon-hello
	RUNTIME DESTINATION usr/bin
)
