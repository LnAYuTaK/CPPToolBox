#include "Loop.h"
#include "EpollLoop.h"
#include "FdEvent.h"
#include "EpollFdEvent.h"

Loop* Loop::New() { return new EpollLoop; }
