#ifndef HARNESS_SPACE_MODEL_USER_H
#define HARNESS_SPACE_MODEL_USER_H

#include "space/support.h"

/**
 * A named model persona, not credentials or a live provider. IDs are host-assigned
 * within a server. Names copy 1..23 lowercase ASCII handle characters. Mutators
 * apply to standalone values; server admission copies and seals identity.
 */
typedef struct ModelUser {
    u64 id;
    char name[MODEL_USER_NAME_CAP];
    u64 modelId;
} ModelUser;

ModelUser ModelUser_0(void);
ModelUser ModelUser_3(u64 id, const char *name, u64 modelId);

#define ModelUser(...) SPACE_CONSTRUCT(ModelUser, __VA_ARGS__)
#define ModelUser_zero() ModelUser_0()

bool ModelUser_setName(ModelUser *self, const char *name);
bool ModelUser_setModelId(ModelUser *self, u64 modelId);

u64 ModelUser_getId(const ModelUser *self);
const char *ModelUser_getName(const ModelUser *self);
u64 ModelUser_getModelId(const ModelUser *self);

bool ModelUser_toString(const ModelUser *self, char *dest, bytes cap, bool *outTruncated);
bool ModelUser_toStringStruct(const ModelUser *self, char *dest, bytes cap, bool *outTruncated);

#endif
