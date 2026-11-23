#ifndef NIO_HARNESS_MEMORY_H
#define NIO_HARNESS_MEMORY_H

#include "vex.h"
#include "nio/mem.h"

MemoryArena *Harness_arena();
fn Harness_arenaShutdown();

#endif
