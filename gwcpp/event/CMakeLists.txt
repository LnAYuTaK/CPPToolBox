aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
add_library(gwevent ${SRC_LIST})
target_link_libraries(gwevent gwbase)
#Install 
install(TARGETS gwevent  DESTINATION lib)
file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/gwcpp/event)
