#include "Loop.h"
#include "EpollFdEvent.h"
#include "EpollLoop.h"
#include "FdEvent.h"
/**
 * @brief 创建事件循环Loop池
 * 
 * @return EpollLoop* 
 */
EpollLoop* Loop::New() { return new EpollLoop; }
