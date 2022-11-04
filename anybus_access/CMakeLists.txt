cmake_minimum_required (VERSION 2.8.1)

######## Project settings ########
PROJECT(anybus_access)
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
	anybus_access

	${SOURCES}
)

TARGET_LINK_LIBRARIES(
	anybus_access
)

######## Install targets ########
INSTALL(TARGETS anybus_access
	RUNTIME DESTINATION usr/bin
)
