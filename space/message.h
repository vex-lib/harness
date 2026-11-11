#ifndef HARNESS_SPACE_MESSAGE_H
#define HARNESS_SPACE_MESSAGE_H

#include "space/support.h"

/**
 * Text is an immutable borrowed byte span, not retained provider output. Caller
 * keeps it live and immutable until all containing histories detach. No decoding
 * or arbitrary-pointer validation. authorId=0 denotes a host/human message;
 * taskId=0 denotes ordinary discussion. Host authenticates authors separately.
 */
typedef struct Message {
    u64 id;
    u64 channelId;
    u64 authorId;
    u64 taskId;
    const char *text;
    bytes length;
} Message;

Message Message_0(void);
Message Message_6(u64 id, u64 channelId, u64 authorId, u64 taskId, const char *text, bytes length);

#define Message(...) SPACE_CONSTRUCT(Message, __VA_ARGS__)
#define Message_zero() Message_0()

u64 Message_getId(const Message *self);
u64 Message_getChannelId(const Message *self);
u64 Message_getAuthorId(const Message *self);
u64 Message_getTaskId(const Message *self);
const char *Message_getText(const Message *self);
bytes Message_getLength(const Message *self);

bool Message_toString(const Message *self, char *dest, bytes cap, bool *outTruncated);
bool Message_toStringStruct(const Message *self, char *dest, bytes cap, bool *outTruncated);

#endif
