cmake_minimum_required (VERSION 2.6)
project (autosar-soad-tcpip)
enable_testing()

include_directories (
	${CMAKE_CURRENT_SOURCE_DIR}/source/include
	${CMAKE_CURRENT_SOURCE_DIR}/externals/soad/source
	${CMAKE_CURRENT_SOURCE_DIR}/externals/tcpip/source
)

list (APPEND SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/externals/soad/source/SoAd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tcpip/source/TcpIp.c
)

add_subdirectory(source/loopback)
