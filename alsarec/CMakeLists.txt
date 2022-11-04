cmake_minimum_required (VERSION 2.8.1)

######## Project settings ########
PROJECT(alsarec)
SET(LICENSE "TBD")

######## Build and include settings ########
include_directories(
	inc
)

link_directories(
	${LINK_DIRECTORIES}
)


file(GLOB SOURCES
	"src/*.c"
)

add_executable(
	alsarec

	${SOURCES}
)

TARGET_LINK_LIBRARIES(
	alsarec
	asound
	pthread
)

######## Install targets ########
INSTALL(TARGETS alsarec
	RUNTIME DESTINATION usr/bin
)
