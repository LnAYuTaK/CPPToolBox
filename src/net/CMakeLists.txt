set(SRC_LIST
        NetAddress.cpp
        Socket.cpp
)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
add_library(gwnet ${SRC_LIST}) 
