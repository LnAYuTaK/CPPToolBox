aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
add_library(gwbase SHARED ${SRC_LIST})
#Install 
install(TARGETS gwbase DESTINATION lib)
file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/gwcpp/base)
