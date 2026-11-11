#ifndef HARNESS_SPACE_CHANNEL_H
#define HARNESS_SPACE_CHANNEL_H

#include "space/support.h"

/**
 * Dedicated model-channel metadata for the proposed server admission contract.
 * Channel ID equals that server's ModelUser ID; IDs are scoped by serverId.
 * Flat message history and HernessServer admission remain unimplemented.
 */
typedef struct Channel {
    u64 id;
    u64 serverId;
    u64 modelUserId;
} Channel;

Channel Channel_0(void);
Channel Channel_2(u64 serverId, u64 modelUserId);

#define Channel(...) SPACE_CONSTRUCT(Channel, __VA_ARGS__)
#define Channel_zero() Channel_0()

u64 Channel_getId(const Channel *self);
u64 Channel_getServerId(const Channel *self);
u64 Channel_getModelUserId(const Channel *self);

bool Channel_toString(const Channel *self, char *dest, bytes cap, bool *outTruncated);
bool Channel_toStringStruct(const Channel *self, char *dest, bytes cap, bool *outTruncated);

#endif
