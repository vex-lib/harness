#ifndef VEX_HARNESS_TEST_H
#define VEX_HARNESS_TEST_H

#include "vex.h"

/**
 * Single suite entry in the consolidated vex/harness automated test runner.
 * name: CLI filter token matching argv[1].
 * run: suite entrypoint executed when selected.
 */
typedef struct SuiteEntry {
    string name;
    fn (*run)();
} SuiteEntry;

/**
 * Local test class proving SPACE_CONSTRUCT dispatches to Class_N by arity.
 * sum: accumulated constructor sum.
 */
typedef struct Probe {
    i32 sum;
} Probe;

#endif
