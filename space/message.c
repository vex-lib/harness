#include "vex.h"
#include "space/message.h"

;;DEFINITION
/**
 * DEFINITION: Message
 *
 * Message is a routed immutable discussion event with optional task association.
 * Planned server history will copy metadata; payload bytes remain caller-owned.
 * This avoids allocator churn but requires explicit lifetime retention by host.
 */

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: Message (space/message.c)
 * ============================================================================
 * STRUCT FIELDS (Mirroring space/message.h):
 * ----------------------------------------------------------------------------
 *   Message {
 *     u64 id;           // event identity
 *     u64 channelId;    // discussion route
 *     u64 authorId;     // sender; zero denotes host
 *     u64 taskId;       // thread; zero denotes ordinary discussion
 *     const char *text;      // borrowed immutable bytes
 *     bytes length;         // payload byte extent
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Constructors:
 *   - Message_0()
 *   - Message_6(id, channelId, authorId, taskId, text, length)
 *   - Message(...) / Message_zero()
 *
 * Getters:
 *   - Message_getId(self)
 *   - Message_getChannelId(self)
 *   - Message_getAuthorId(self)
 *   - Message_getTaskId(self)
 *   - Message_getText(self)
 *   - Message_getLength(self)
 *
 * Projections:
 *   - Message_toString(self, dest, cap, outTruncated)
 *   - Message_toStringStruct(self, dest, cap, outTruncated)
 *
 * Empty/null/oversized spans reject to zero without payload access. Getters
 * return safe defaults on nullptr. Payload retention is the caller's duty.
 * ============================================================================
 */

;;INTENTION("message metadata and payload admission are immutable snapshots; mutation requires a new event per the Conflict Triage Law + Single Class Per File Law (Java Law)")

/**
 * Returns an empty event, not an admissible server-history entry.
 */
Message Message_0() {
    return (Message) {0};
}

/**
 * Admits live immutable text with representable length, without copy/decoding.
 */
Message Message_6(u64 id, u64 channelId, u64 authorId, u64 taskId, const char *text, bytes length) {
    if (id == 0 || channelId == 0 || text == nullptr || length == 0 || length > PTRDIFF_MAX) {
        THROW("message: invalid input");
        return Message_0();
    }
    return (Message) {id, channelId, authorId, taskId, text, length};
}

/**
 * Returns the immutable event identity, or zero for nullptr.
 */
;;GETTER
u64 Message_getId(const Message *self) {
    if (self == nullptr)
        return 0;
    return (*self).id;
}

/**
 * Returns the destination identity without following a pointer chain.
 */
;;GETTER
u64 Message_getChannelId(const Message *self) {
    if (self == nullptr)
        return 0;
    return (*self).channelId;
}

/**
 * Returns the sender identity; zero is a host marker, not authentication proof.
 */
;;GETTER
u64 Message_getAuthorId(const Message *self) {
    if (self == nullptr)
        return 0;
    return (*self).authorId;
}

/**
 * Returns the associated task identity without nesting history.
 */
;;GETTER
u64 Message_getTaskId(const Message *self) {
    if (self == nullptr)
        return 0;
    return (*self).taskId;
}

/**
 * Returns borrowed text, valid only under the caller's retention contract.
 */
;;GETTER
const char *Message_getText(const Message *self) {
    if (self == nullptr)
        return nullptr;
    return (*self).text;
}

/**
 * Returns the byte count, including embedded NUL bytes; nullptr yields zero.
 */
;;GETTER
bytes Message_getLength(const Message *self) {
    if (self == nullptr)
        return 0;
    return (*self).length;
}

/**
 * Formats a summary without traversing or formatting the payload.
 */
bool Message_toString(const Message *self, char *dest, bytes cap, bool *outTruncated) {
    return self == nullptr ? Space_format(dest, cap, outTruncated, "nullptr") :
        Space_format(dest, cap, outTruncated, "message %llu (%zu bytes)",
            (u64) Message_getId(self), Message_getLength(self));
}

/**
 * Formats metadata followed by escaped payload bytes. The draft length field
 * is embedded in the text label; declaration-order projection remains pending.
 */
bool Message_toStringStruct(const Message *self, char *dest, bytes cap, bool *outTruncated) {
    if (self == nullptr)
        return Space_format(dest, cap, outTruncated, "nullptr");
    if (!Space_format(dest, cap, outTruncated,
        "Message{id=%llu,channelId=%llu,authorId=%llu,taskId=%llu,text[length=%zu]=\"",
        (u64) Message_getId(self), (u64) Message_getChannelId(self),
        (u64) Message_getAuthorId(self), (u64) Message_getTaskId(self), Message_getLength(self)))
        return false;
    return Space_quote(Message_getText(self), Message_getLength(self), dest, cap, outTruncated);
}
