# 添加头文件路径，会检索目录中的所有头文件
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../base
                    ${CMAKE_CURRENT_SOURCE_DIR}/../db
                    ${CMAKE_CURRENT_SOURCE_DIR}/../device
                    ${CMAKE_CURRENT_SOURCE_DIR}/../net
                    ${CMAKE_CURRENT_SOURCE_DIR}/../event)

# 把源码编译成一个可执行文件
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
# 添加链接库，动态和静态都行
target_link_libraries(${PROJECT_NAME}  gwevent gwdevice gwdb  gwnet gwbase)
target_link_libraries(${PROJECT_NAME}  paho-mqttpp3  paho-mqtt3a)
# target_link_libraries(${PROJECT_NAME}   libhpsocket.a)
target_link_libraries(${PROJECT_NAME}  -lpthread -lrt -ldl -lm)