aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
add_library(gwdevice ${SRC_LIST})
target_link_libraries(gwdevice gwevent gwbase)

install(TARGETS gwdevice DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/gwcpp/device)

