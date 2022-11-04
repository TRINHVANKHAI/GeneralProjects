cmake_minimum_required (VERSION 2.8.1)

######## Project settings ########
PROJECT(pcap-hello)
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
	pcap-hello

	${SOURCES}
)

TARGET_LINK_LIBRARIES(
	pcap-hello
	pcap
)

######## Install targets ########
INSTALL(TARGETS pcap-hello
	RUNTIME DESTINATION usr/bin
)
