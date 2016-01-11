cmake_minimum_required (VERSION 2.6)
project (echo)

include_directories (
	${CMAKE_CURRENT_SOURCE_DIR}/.
    ${CMAKE_CURRENT_SOURCE_DIR}/config
)

list (APPEND SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/config/SoAd_Cfg.c
	${CMAKE_CURRENT_SOURCE_DIR}/config/TcpIp_Cfg.c
	${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

add_executable (echo ${SOURCES})
