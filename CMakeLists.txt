cmake_minimum_required (VERSION 2.6)
project (autosar-soad-tcpip)

include_directories (externals/soad/source
                     externals/tcpip/source
                     source
                     source/config
                     source/include)

set (SOURCES externals/soad/source/SoAd.c
             externals/tcpip/source/TcpIp.c
             source/config/SoAd_Cfg.c
             source/config/TcpIp_Cfg.c
             source/main.c)

add_executable (main ${SOURCES})

set_target_properties (main PROPERTIES LINK_FLAGS -Wl,-lcunit)

