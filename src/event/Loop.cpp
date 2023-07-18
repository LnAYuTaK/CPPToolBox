#include "Loop.h"
#include "EpollFdEvent.h"
#include "EpollLoop.h"
#include "FdEvent.h"

EpollLoop* Loop::New() { return new EpollLoop; }
