set(SRC_LIST
        Address.cpp
        Socket.cpp
        Acceptor.cpp
        Connector.cpp
        TcpConnection.cpp
        TcpClient.cpp
)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
add_library(gwnet ${SRC_LIST}) 
