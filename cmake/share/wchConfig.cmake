# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

message("start")


macro(include_modules location)

    list(PREPEND CMAKE_MODULE_PATH ${WCH_BASE}/cmake/modules)

    if(NOT DEFINED APPLICATION_SOURCE_DIR)
        set(APPLICATION_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH
            "Application Source Directory"
        )
    endif()

    if(NOT DEFINED APPLICATION_BINARY_DIR)
        set(APPLICATION_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH
            "Application Binary Directory"
        )
    endif()  

    message("set app src dir " ${APPLICATION_SOURCE_DIR})
    message("set app bin dir " ${CMAKE_CURRENT_BINARY_DIR})
    
    set(__build_dir ${APPLICATION_BINARY_DIR}/wch)
    set(PROJECT_BINARY_DIR ${__build_dir})


endmacro()

set(ENV_WCH_BASE $ENV{WCH_BASE})

message($ENV{WCH_BASE})

if(DEFINED ENV_WCH_BASE)
    message("ENV WCH BASE: " ${ENV_WCH_BASE})
    include_modules("WCH base")

else()
    message("not found")
endif()