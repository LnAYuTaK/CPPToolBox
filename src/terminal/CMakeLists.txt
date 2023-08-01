
set(SRC_LIST
        KeyEventScanner.cpp
        Nodes.cpp
        Session.cpp
        TerminalCommands.cpp
        TerminalKeyEvents.cpp
        TerminalNodes.cpp
        Terminal.cpp
)

# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)

add_library(gwterminal ${SRC_LIST})
target_link_libraries(gwterminal gwbase)
