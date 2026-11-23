#include "vex.h"
#include "nio/harness_memory.h"

;;DEFINITION
/**
 * ============================================================================
 * DEFINITION: HarnessMemory (harness/nio/harness_memory.c)
 * ============================================================================
 * Per-library MemoryArena owner for Harness, backed by Relational Engine's
 * MemoryPool. Starts at zero OS pages and grows on demand until OOM.
 * ============================================================================
 */

;;OVERVIEW
/**
 * ============================================================================
 * MODULE: HarnessMemory (nio/harness_memory)
 * ============================================================================
 * Lazily creates and shuts down Harness's dedicated MemoryArena.
 * ============================================================================
 */

static MemoryArena *s_harnessArena = nullptr;

/** Lazily create and return Harness's page-pooled MemoryArena. */
MemoryArena *Harness_arena() {
    if (s_harnessArena == nullptr)
        s_harnessArena = MemoryArena();
    return s_harnessArena;
}

/** Destroy Harness's MemoryArena and return all pages to MemoryPool. */
fn Harness_arenaShutdown() {
    if (s_harnessArena != nullptr) {
        MemoryArena_destroy(s_harnessArena);
        s_harnessArena = nullptr;
    }
}
