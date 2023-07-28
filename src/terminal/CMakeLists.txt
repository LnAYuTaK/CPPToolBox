

set(SRC_LIST
  dir_node.cpp
  func_node.cpp
  key_event_scanner.cpp
  split_cmdline.cpp
#   terminal_commands.cpp
  terminal_nodes.cpp
  terminal_key_events.cpp
  terminal.cpp
  util_string.cpp
)

# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)

add_library(gwterminal ${SRC_LIST})
target_link_libraries(gwterminal gwbase)
