aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
add_library(gwnet ${SRC_LIST}) 
target_link_libraries(gwnet PRIVATE hpsocket)