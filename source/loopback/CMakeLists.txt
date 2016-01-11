cmake_minimum_required (VERSION 2.6)
project (loopback)
enable_testing()

include_directories (
	${CMAKE_CURRENT_SOURCE_DIR}/.
    ${CMAKE_CURRENT_SOURCE_DIR}/config
)

list (APPEND SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/config/SoAd_Cfg.c
	${CMAKE_CURRENT_SOURCE_DIR}/config/TcpIp_Cfg.c
	${CMAKE_CURRENT_SOURCE_DIR}/main.c
 )

add_executable (loopback ${SOURCES})

set_target_properties (loopback PROPERTIES LINK_FLAGS -Wl,-lcunit)

add_test(NAME    loopback
         COMMAND loopback)