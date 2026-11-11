#ifndef HARNESS_SPACE_TASK_H
#define HARNESS_SPACE_TASK_H

#include "space/support.h"

/**
 * Permissions are a host-defined bitset; this core can narrow but never grant
 * execution authority. It performs NO external actions. Lifecycle derives only
 * through start/finish/cancel and server-scoped admission/delegation.
 */
#define TASK_QUEUED 1u
#define TASK_RUNNING 2u
#define TASK_DONE 3u
#define TASK_CANCELLED 4u

typedef struct Task {
    u64 id;
    u64 channelId;
    u64 requesterId;
    u64 assigneeId;
    u64 parentId;
    u64 permissions;
    u32 state;
} Task;

Task Task_0(void);
Task Task_5(u64 id, u64 channelId, u64 requesterId, u64 assigneeId, u64 permissions);

#define Task(...) SPACE_CONSTRUCT(Task, __VA_ARGS__)
#define Task_zero() Task_0()

bool Task_start(Task *self, u64 actorId);
bool Task_finish(Task *self, u64 actorId);

/**
 * Cancellation is host-only admission; the caller must enforce host authority.
 */
bool Task_cancel(Task *self);
bool Task_delegate(const Task *parent, u64 actorId, u64 id, u64 channelId,
                   u64 assigneeId, u64 permissions, Task *dest);

u64 Task_getId(const Task *self);
u64 Task_getChannelId(const Task *self);
u64 Task_getRequesterId(const Task *self);
u64 Task_getAssigneeId(const Task *self);
u64 Task_getParentId(const Task *self);
u64 Task_getPermissions(const Task *self);
u32 Task_getState(const Task *self);

bool Task_toString(const Task *self, char *dest, bytes cap, bool *outTruncated);
bool Task_toStringStruct(const Task *self, char *dest, bytes cap, bool *outTruncated);

#endif
