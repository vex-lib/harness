/**
 * Consolidated automated owner test suite for vex/harness.
 * Usage: harness_test [channel|message|model_user|support|task]
 */

#include "vex.h"
#include "test_support.h"

#include <stdio.h>
#include <string.h>

#include "space/channel.h"
#include "space/message.h"
#include "space/model_user.h"
#include "space/support.h"
#include "space/task.h"
#include "test/harness_test.h"

static i32 g_failures = 0;

#define CHECK(tag, name, cond)                                            \
    do {                                                                  \
        if (cond) {                                                       \
            printf("[%s] PASS %s\n", tag, name);                          \
        } else {                                                          \
            printf("[%s] FAIL %s\n", tag, name);                          \
            g_failures++;                                                 \
        }                                                                 \
    } while (0)

/** Proves Channel identity, server scoping, boundary IDs, null-safe getters, and projections. */
static fn suite_channel() {
    printf("=== Channel owner suite ===\n");

    Channel zero = Channel_0();
    Channel chooserZero = Channel();
    CHECK("channel", "zero is all-zero", zero.id == 0 && zero.serverId == 0 && zero.modelUserId == 0);
    CHECK("channel", "chooser() == _0", chooserZero.id == zero.id && chooserZero.serverId == zero.serverId);
    CHECK("channel", "zero() == _0", Channel_zero().id == 0);

    Channel c = Channel(11, 22);
    CHECK("channel", "id is the persona", c.id == 22);
    CHECK("channel", "serverId recorded", c.serverId == 11);
    CHECK("channel", "modelUserId recorded", c.modelUserId == 22);
    CHECK("channel", "chain 2-arg form", Channel(11, 22).id == Channel_2(11, 22).id);

    Channel big = Channel(UINT64_MAX, UINT64_MAX);
    CHECK("channel", "boundary max accepted",
          big.serverId == UINT64_MAX && big.modelUserId == UINT64_MAX && big.id == UINT64_MAX);

    CHECK("channel", "zero server rejected", Channel(0, 22).id == 0 && Channel(0, 22).serverId == 0);
    CHECK("channel", "zero persona rejected", Channel(11, 0).id == 0 && Channel(11, 0).modelUserId == 0);

    CHECK("channel", "getters null self",
          Channel_getId(nullptr) == 0 && Channel_getServerId(nullptr) == 0 &&
          Channel_getModelUserId(nullptr) == 0);
    CHECK("channel", "getters live",
          Channel_getId(&c) == 22 && Channel_getServerId(&c) == 11 &&
          Channel_getModelUserId(&c) == 22);
    CHECK("channel", "same id, different servers are distinct",
          Channel(1, 9).id == Channel(2, 9).id && Channel(1, 9).serverId != Channel(2, 9).serverId);

    utf8 buf[128];
    bool cut = false;
    CHECK("channel", "toString null self",
          Channel_toString(nullptr, buf, sizeof(buf), &cut) && strcmp(buf, "nullptr") == 0);
    CHECK("channel", "toString value",
          Channel_toString(&c, buf, sizeof(buf), &cut) && strcmp(buf, "channel 11/22") == 0);
    CHECK("channel", "toStringStruct null self",
          Channel_toStringStruct(nullptr, buf, sizeof(buf), &cut) && strcmp(buf, "nullptr") == 0);
    CHECK("channel", "toStringStruct fields",
          Channel_toStringStruct(&c, buf, sizeof(buf), &cut) &&
          strcmp(buf, "Channel{id=22,serverId=11,modelUserId=22}") == 0 && !cut);
    CHECK("channel", "toString truncates", !Channel_toString(&c, buf, 4, &cut) && cut);
    CHECK("channel", "toStringStruct truncates", !Channel_toStringStruct(&c, buf, 5, &cut) && cut);
}

/** Proves Message borrowed payload identity, embedded NUL spans, rejections, and projections. */
static fn suite_message() {
    printf("=== Message owner suite ===\n");

    Message zero = Message_0();
    CHECK("message", "zero is all-zero",
          zero.id == 0 && zero.channelId == 0 && zero.text == nullptr && zero.length == 0);
    CHECK("message", "chooser() == _0", Message().id == 0 && Message().text == nullptr);
    CHECK("message", "zero() == _0", Message_zero().id == 0);

    const utf8 *payload = "hello";
    Message m = Message(5, 6, 0, 0, payload, 5);
    CHECK("message", "chooser 6-arg id", m.id == 5);
    CHECK("message", "chooser 6-arg channel", m.channelId == 6);
    CHECK("message", "author 0 is allowed (host)", m.authorId == 0);
    CHECK("message", "task 0 is allowed (plain)", m.taskId == 0);
    CHECK("message", "text is borrowed, not copied", m.text == payload);
    CHECK("message", "length recorded", m.length == 5);
    CHECK("message", "same 6-arg via _6", Message_6(5, 6, 1, 2, payload, 5).authorId == 1);

    Message bounded = Message(UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, "x", 1);
    CHECK("message", "boundary max ids accepted",
          bounded.id == UINT64_MAX && bounded.taskId == UINT64_MAX);

    CHECK("message", "zero id rejected",
          Message(0, 6, 0, 0, payload, 5).id == 0 && Message(0, 6, 0, 0, payload, 5).text == nullptr);
    CHECK("message", "zero channel rejected", Message(5, 0, 0, 0, payload, 5).id == 0);
    CHECK("message", "null text rejected", Message(5, 6, 0, 0, nullptr, 5).id == 0);
    CHECK("message", "zero length rejected", Message(5, 6, 0, 0, payload, 0).id == 0);
    CHECK("message", "oversized length rejected",
          Message(5, 6, 0, 0, payload, (bytes) PTRDIFF_MAX + 1u).id == 0);

    const utf8 raw[4] = { 'a', '\0', 'b', 'c' };
    Message nul = Message(1, 2, 0, 0, raw, 4);
    CHECK("message", "embedded NUL kept",
          Message_getLength(&nul) == 4 && Message_getText(&nul)[1] == '\0' &&
          Message_getText(&nul)[3] == 'c');

    CHECK("message", "getters null self",
          Message_getId(nullptr) == 0 && Message_getChannelId(nullptr) == 0 &&
          Message_getAuthorId(nullptr) == 0 && Message_getTaskId(nullptr) == 0 &&
          Message_getText(nullptr) == nullptr && Message_getLength(nullptr) == 0);
    CHECK("message", "getters live",
          Message_getId(&m) == 5 && Message_getChannelId(&m) == 6 &&
          Message_getText(&m) == payload && Message_getLength(&m) == 5);

    utf8 buf[256];
    bool cut = false;
    CHECK("message", "toString null self",
          Message_toString(nullptr, buf, sizeof(buf), &cut) && strcmp(buf, "nullptr") == 0);
    CHECK("message", "toString value",
          Message_toString(&m, buf, sizeof(buf), &cut) && strcmp(buf, "message 5 (5 bytes)") == 0);
    CHECK("message", "toStringStruct null self",
          Message_toStringStruct(nullptr, buf, sizeof(buf), &cut) && strcmp(buf, "nullptr") == 0);
    CHECK("message", "toStringStruct fields",
          Message_toStringStruct(&m, buf, sizeof(buf), &cut) &&
          strcmp(buf, "Message{id=5,channelId=6,authorId=0,taskId=0,text[length=5]=\"hello\"}") == 0 && !cut);

    const utf8 *esc = "a\nb.sh\tc\"d\\";
    Message e = Message(1, 2, 0, 0, esc, strlen(esc));
    CHECK("message", "toStringStruct escapes payload",
          Message_toStringStruct(&e, buf, sizeof(buf), &cut) &&
          strcmp(buf, "Message{id=1,channelId=2,authorId=0,taskId=0,text[length=8]=\"a\\nb\\tc\\\"d\\\\\"}") == 0);
    CHECK("message", "toStringStruct escapes NUL",
          Message_toStringStruct(&nul, buf, sizeof(buf), &cut) && strstr(buf, "\\x00") != nullptr);
    CHECK("message", "toString truncates", !Message_toString(&m, buf, 6, &cut) && cut);
    CHECK("message", "toStringStruct truncates", !Message_toStringStruct(&m, buf, 12, &cut) && cut);
    CHECK("message", "toStringStruct null flag tolerated",
          Message_toStringStruct(&m, buf, sizeof(buf), nullptr));
}

/** Compares two ModelUser instances for byte-exact equality. */
static bool sameUser(const ModelUser *a, const ModelUser *b) {
    return ModelUser_getId(a) == ModelUser_getId(b) &&
           ModelUser_getModelId(a) == ModelUser_getModelId(b) &&
           memcmp(ModelUser_getName(a), ModelUser_getName(b), MODEL_USER_NAME_CAP) == 0;
}

/** Returns true when a ModelUser value is all-zero. */
static bool isZeroUser(ModelUser v) {
    return v.id == 0 && v.modelId == 0 && v.name[0] == '\0';
}

/** Proves ModelUser persona identity, handle validation, reject-and-preserve setters, and projections. */
static fn suite_model_user() {
    printf("=== ModelUser owner suite ===\n");
    const utf8 *n23 = "aaaaaaaaaaaaaaaaaaaaaaa";

    ModelUser zero = ModelUser_0();
    ModelUser chooserZero = ModelUser();
    CHECK("model_user", "zero id/name/model", zero.id == 0 && zero.name[0] == '\0' && zero.modelId == 0);
    CHECK("model_user", "chooser() == zero", sameUser(&zero, &chooserZero));
    ModelUser zeroAlias = ModelUser_zero();
    CHECK("model_user", "zero() == _0", sameUser(&zeroAlias, &zero));

    ModelUser u = ModelUser(7, "vex", 3);
    CHECK("model_user", "chooser 3-arg id", u.id == 7);
    CHECK("model_user", "chooser 3-arg name", strcmp(u.name, "vex") == 0);
    CHECK("model_user", "chooser 3-arg modelId", u.modelId == 3);
    CHECK("model_user", "name is zero-padded", u.name[4] == '\0' && u.name[23] == '\0');

    ModelUser big = ModelUser(UINT64_MAX, n23, UINT64_MAX);
    CHECK("model_user", "boundary max id/model accepted",
          big.id == UINT64_MAX && big.modelId == UINT64_MAX);

    CHECK("model_user", "zero id rejected", isZeroUser(ModelUser_3(0, "vex", 3)));
    CHECK("model_user", "zero modelId rejected", isZeroUser(ModelUser_3(1, "vex", 0)));
    CHECK("model_user", "null name rejected", isZeroUser(ModelUser_3(1, nullptr, 3)));
    CHECK("model_user", "empty name rejected", isZeroUser(ModelUser_3(1, "", 3)));
    CHECK("model_user", "uppercase name rejected", isZeroUser(ModelUser_3(1, "Vex", 3)));
    CHECK("model_user", "overlong name rejected",
          isZeroUser(ModelUser_3(1, "aaaaaaaaaaaaaaaaaaaaaaaa", 3)));

    ModelUser s = ModelUser(1, "longername", 2);
    CHECK("model_user", "setName accepts",
          ModelUser_setName(&s, "short") && strcmp(s.name, "short") == 0);
    CHECK("model_user", "setName re-pads leftovers", s.name[5] == '\0' && s.name[10] == '\0');
    ModelUser before = s;
    CHECK("model_user", "setName rejects null", !ModelUser_setName(&s, nullptr));
    CHECK("model_user", "setName rejects uppercase", !ModelUser_setName(&s, "Bad"));
    CHECK("model_user", "setName rejects overlong",
          !ModelUser_setName(&s, "aaaaaaaaaaaaaaaaaaaaaaaa"));
    CHECK("model_user", "setName preserves on reject", sameUser(&s, &before));
    CHECK("model_user", "setName rejects null self", !ModelUser_setName(nullptr, "ok"));

    CHECK("model_user", "setModelId rejects zero",
          !ModelUser_setModelId(&s, 0) && s.modelId == before.modelId);
    CHECK("model_user", "setModelId rejects null self", !ModelUser_setModelId(nullptr, 9));
    CHECK("model_user", "setModelId accepts", ModelUser_setModelId(&s, 99) && s.modelId == 99);

    CHECK("model_user", "getters null self",
          ModelUser_getId(nullptr) == 0 && ModelUser_getModelId(nullptr) == 0 &&
          ModelUser_getName(nullptr) == nullptr);
    CHECK("model_user", "getName on empty value is not null",
          ModelUser_getName(&zero) != nullptr && ModelUser_getName(&zero)[0] == '\0');
    CHECK("model_user", "getters live",
          ModelUser_getId(&s) == 1 && ModelUser_getModelId(&s) == 99 &&
          strcmp(ModelUser_getName(&s), "short") == 0);

    utf8 buf[128];
    bool cut = false;
    CHECK("model_user", "toString null self",
          ModelUser_toString(nullptr, buf, sizeof(buf), &cut) && strcmp(buf, "nullptr") == 0);
    CHECK("model_user", "toString value",
          ModelUser_toString(&s, buf, sizeof(buf), &cut) && strcmp(buf, "@short") == 0 && !cut);
    CHECK("model_user", "toStringStruct null self",
          ModelUser_toStringStruct(nullptr, buf, sizeof(buf), &cut) && strcmp(buf, "nullptr") == 0);
    CHECK("model_user", "toStringStruct fields",
          ModelUser_toStringStruct(&s, buf, sizeof(buf), &cut) &&
          strcmp(buf, "ModelUser{id=1,name=\"short\",modelId=99}") == 0 && !cut);
    CHECK("model_user", "toString truncates", !ModelUser_toString(&s, buf, 3, &cut) && cut);
    CHECK("model_user", "toStringStruct truncates", !ModelUser_toStringStruct(&s, buf, 8, &cut) && cut);
    CHECK("model_user", "toStringStruct null dest rejects",
          !ModelUser_toStringStruct(&s, nullptr, 8, &cut) && cut);
    CHECK("model_user", "toStringStruct null flag tolerated",
          ModelUser_toStringStruct(&s, buf, sizeof(buf), nullptr));
}

enum {
    A0 = SPACE_COUNT(),
    A1 = SPACE_COUNT(1),
    A2 = SPACE_COUNT(1, 2),
    A3 = SPACE_COUNT(1, 2, 3),
    A4 = SPACE_COUNT(1, 2, 3, 4),
    A5 = SPACE_COUNT(1, 2, 3, 4, 5),
    A6 = SPACE_COUNT(1, 2, 3, 4, 5, 6),
    A7 = SPACE_COUNT(1, 2, 3, 4, 5, 6, 7),
    A8 = SPACE_COUNT(1, 2, 3, 4, 5, 6, 7, 8)
};
_Static_assert(A0 == 0 && A1 == 1 && A2 == 2 && A3 == 3 && A4 == 4 &&
               A5 == 5 && A6 == 6 && A7 == 7 && A8 == 8, "arity counter must rank 0..8");

/** Zero-arg Probe constructor fixture. */
static Probe Probe_0()
{
    return (Probe) { 0 };
}

/** Two-arg Probe constructor fixture. */
static Probe Probe_2(i32 a, i32 b)
{
    return (Probe) { a + b };
}

/** Three-arg Probe constructor fixture. */
static Probe Probe_3(i32 a, i32 b, i32 c)
{
    return (Probe) { a + b + c };
}

#define Probe(...) SPACE_CONSTRUCT(Probe, __VA_ARGS__)

/** Proves SPACE_CONSTRUCT arity dispatch, handle validation, formatting, and quote escaping. */
static fn suite_support() {
    printf("=== Space support owner suite ===\n");

    CHECK("support", "chooser 0 args selects Probe_0", Probe().sum == 0);
    CHECK("support", "chooser 2 args selects Probe_2", Probe(2, 3).sum == 5);
    CHECK("support", "chooser 3 args selects Probe_3", Probe(1, 2, 3).sum == 6);

    CHECK("support", "name nullptr rejected", !Space_nameValid(nullptr));
    CHECK("support", "name empty rejected", !Space_nameValid(""));
    CHECK("support", "name single utf8 ok", Space_nameValid("a"));
    CHECK("support", "name digits ok", Space_nameValid("0"));
    CHECK("support", "name dash underscore ok", Space_nameValid("a-b_c"));
    CHECK("support", "name 22 chars ok", Space_nameValid("aaaaaaaaaaaaaaaaaaaaaa"));
    CHECK("support", "name 23 chars ok", Space_nameValid("aaaaaaaaaaaaaaaaaaaaaaa"));
    CHECK("support", "name 24 chars rejected", !Space_nameValid("aaaaaaaaaaaaaaaaaaaaaaaa"));
    CHECK("support", "name uppercase rejected", !Space_nameValid("Ab"));
    CHECK("support", "name dot rejected", !Space_nameValid("a.b.sh"));
    CHECK("support", "name at rejected", !Space_nameValid("a@b.sh"));
    CHECK("support", "name space rejected", !Space_nameValid("a b.sh"));
    CHECK("support", "name DEL rejected", !Space_nameValid("\x7f"));
    CHECK("support", "name high byte rejected", !Space_nameValid("\x80"));
    CHECK("support", "name stops at embedded NUL", Space_nameValid("abc\0def"));

    utf8 buf[64];
    bool cut = true;
    CHECK("support", "format fits",
          Space_format(buf, sizeof(buf), &cut, "%d", 42) && !cut && strcmp(buf, "42") == 0);
    CHECK("support", "format exact fit",
          Space_format(buf, 3, &cut, "ab") && !cut && strcmp(buf, "ab") == 0);
    CHECK("support", "format one short truncates",
          !Space_format(buf, 2, &cut, "ab") && cut && strcmp(buf, "a") == 0);
    CHECK("support", "format null dest rejects", !Space_format(nullptr, 8, &cut, "x") && cut);
    CHECK("support", "format zero cap rejects", !Space_format(buf, 0, &cut, "x") && cut);
    CHECK("support", "format null flag tolerated",
          Space_format(buf, sizeof(buf), nullptr, "ok") && strcmp(buf, "ok") == 0);

    buf[0] = '\0';
    bool ok = Space_quote("hi", 2, buf, sizeof(buf), &cut);
    CHECK("support", "quote plain", ok && !cut && strcmp(buf, "hi\"}") == 0);
    buf[0] = '\0';
    const utf8 *esc = "a\nb.sh\tc\"d\\";
    ok = Space_quote(esc, strlen(esc), buf, sizeof(buf), &cut);
    CHECK("support", "quote escapes", ok && strcmp(buf, "a\\nb\\tc\\\"d\\\\\"}") == 0);
    buf[0] = '\0';
    ok = Space_quote("\x01\x7f\x80", 3, buf, sizeof(buf), &cut);
    CHECK("support", "quote control/high bytes", ok && strcmp(buf, "\\x01\\x7f\\x80\"}") == 0);
    buf[0] = '\0';
    ok = Space_quote("", 0, buf, sizeof(buf), &cut);
    CHECK("support", "quote empty span", ok && strcmp(buf, "\"}") == 0);
    buf[0] = '\0';
    ok = Space_quote("abcdef", 6, buf, 6, &cut);
    CHECK("support", "quote truncates", !ok && cut && buf[0] == 'a');
    buf[0] = '\0';
    ok = Space_quote("", 0, buf, 2, &cut);
    CHECK("support", "quote no room for closing", !ok && cut);
}

#define PERM_READ 1u
#define PERM_WRITE 2u
#define PERM_ADMIN 4u

/** Proves Task lifecycle transitions, delegation narrowing, authority checks, and projections. */
static fn suite_task() {
    printf("=== Task owner suite ===\n");

    Task zero = Task_0();
    CHECK("task", "zero is all-zero", zero.id == 0 && zero.state == 0);
    CHECK("task", "chooser() == _0", Task().id == 0);
    CHECK("task", "zero() == _0", Task_zero().id == 0);

    Task root = Task(100, 7, 42, 9, PERM_READ | PERM_WRITE);
    CHECK("task", "chooser 5-arg id", root.id == 100);
    CHECK("task", "channel recorded", Task_getChannelId(&root) == 7);
    CHECK("task", "requester recorded", Task_getRequesterId(&root) == 42);
    CHECK("task", "assignee recorded", Task_getAssigneeId(&root) == 9);
    CHECK("task", "permissions recorded", Task_getPermissions(&root) == (PERM_READ | PERM_WRITE));
    CHECK("task", "root parent is zero", Task_getParentId(&root) == 0);
    CHECK("task", "starts queued", Task_getState(&root) == TASK_QUEUED);

    CHECK("task", "zero id rejected", Task(0, 7, 1, 9, PERM_READ).id == 0);
    CHECK("task", "zero channel rejected", Task(1, 0, 1, 9, PERM_READ).id == 0);
    CHECK("task", "zero assignee rejected", Task(1, 7, 1, 0, PERM_READ).id == 0);
    CHECK("task", "requester 0 (host) allowed", Task(1, 7, 0, 9, PERM_READ).id == 1);
    Task maxed = Task(UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX);
    CHECK("task", "boundary max accepted",
          maxed.id == UINT64_MAX && maxed.permissions == UINT64_MAX);

    Task t = root;
    CHECK("task", "start rejects wrong actor", !Task_start(&t, 42) && Task_getState(&t) == TASK_QUEUED);
    CHECK("task", "start rejects actor 0", !Task_start(&t, 0) && Task_getState(&t) == TASK_QUEUED);
    CHECK("task", "start rejects null self", !Task_start(nullptr, 9));
    CHECK("task", "start accepts assignee", Task_start(&t, 9) && Task_getState(&t) == TASK_RUNNING);
    CHECK("task", "start twice rejected", !Task_start(&t, 9) && Task_getState(&t) == TASK_RUNNING);
    CHECK("task", "finish rejects wrong actor", !Task_finish(&t, 42) && Task_getState(&t) == TASK_RUNNING);
    CHECK("task", "finish accepts assignee", Task_finish(&t, 9) && Task_getState(&t) == TASK_DONE);
    CHECK("task", "finish stays done", !Task_finish(&t, 9) && Task_getState(&t) == TASK_DONE);
    CHECK("task", "cancel done rejected", !Task_cancel(&t) && Task_getState(&t) == TASK_DONE);

    Task q = root;
    CHECK("task", "cancel queued accepted", Task_cancel(&q) && Task_getState(&q) == TASK_CANCELLED);
    CHECK("task", "cancel twice rejected", !Task_cancel(&q) && Task_getState(&q) == TASK_CANCELLED);
    Task emptyTask = Task_0();
    CHECK("task", "cancel null self rejected", !Task_cancel(nullptr) && !Task_cancel(&emptyTask));
    Task r = root;
    CHECK("task", "start then cancel",
          Task_start(&r, 9) && Task_cancel(&r) && Task_getState(&r) == TASK_CANCELLED);

    Task parent = root;
    CHECK("task", "delegate requires running",
          !Task_delegate(&parent, 9, 200, 7, 3, PERM_READ, &(Task) { 0 }) &&
          Task_getState(&parent) == TASK_QUEUED);
    CHECK("task", "start parent", Task_start(&parent, 9));

    Task child;
    memset(&child, 0xAB, sizeof(child));
    bool ok = Task_delegate(&parent, 9, 200, 8, 3, PERM_READ, &child);
    CHECK("task", "delegate child id", ok && Task_getId(&child) == 200);
    CHECK("task", "delegate child channel", Task_getChannelId(&child) == 8);
    CHECK("task", "delegate child assignee", Task_getAssigneeId(&child) == 3);
    CHECK("task", "delegate child requester is actor", Task_getRequesterId(&child) == 9);
    CHECK("task", "delegate child parent", Task_getParentId(&child) == 100);
    CHECK("task", "delegate child permissions", Task_getPermissions(&child) == PERM_READ);
    CHECK("task", "delegate child queued", Task_getState(&child) == TASK_QUEUED);
    CHECK("task", "parent unchanged",
          Task_getState(&parent) == TASK_RUNNING && Task_getParentId(&parent) == 0);

    Task deny = (Task) { .id = 55 };
    CHECK("task", "delegate rejects superset permissions",
          !Task_delegate(&parent, 9, 201, 8, 3, PERM_READ | PERM_WRITE | PERM_ADMIN, &deny));
    CHECK("task", "delegate allows equal permissions",
          Task_delegate(&parent, 9, 202, 8, 3, PERM_READ | PERM_WRITE, &(Task) { 0 }));
    CHECK("task", "delegate rejects wrong actor",
          !Task_delegate(&parent, 42, 201, 8, 3, PERM_READ, &deny));
    CHECK("task", "delegate rejects actor 0",
          !Task_delegate(&parent, 0, 201, 8, 3, PERM_READ, &deny));
    CHECK("task", "delegate rejects reused id",
          !Task_delegate(&parent, 9, 100, 8, 3, PERM_READ, &deny));
    CHECK("task", "delegate rejects zero child id",
          !Task_delegate(&parent, 9, 0, 8, 3, PERM_READ, &deny));
    CHECK("task", "delegate rejects zero channel",
          !Task_delegate(&parent, 9, 201, 0, 3, PERM_READ, &deny));
    CHECK("task", "delegate rejects zero assignee",
          !Task_delegate(&parent, 9, 201, 8, 0, PERM_READ, &deny));
    CHECK("task", "delegate rejects null parent",
          !Task_delegate(nullptr, 9, 201, 8, 3, PERM_READ, &deny));
    CHECK("task", "delegate rejects null dest",
          !Task_delegate(&parent, 9, 201, 8, 3, PERM_READ, nullptr));
    CHECK("task", "delegate rejects dest == parent",
          !Task_delegate(&parent, 9, 201, 8, 3, PERM_READ, &parent));
    CHECK("task", "rejections preserve parent", Task_getState(&parent) == TASK_RUNNING);
    CHECK("task", "rejections preserve dest",
          deny.id == 55 && deny.state == 0 && deny.permissions == 0);

    CHECK("task", "start child", Task_start(&child, 3));
    Task grand;
    CHECK("task", "grandchild delegated",
          Task_delegate(&child, 3, 300, 9, 4, PERM_READ, &grand) &&
          Task_getParentId(&grand) == 200 && Task_getPermissions(&grand) == PERM_READ);

    CHECK("task", "getters null self",
          Task_getId(nullptr) == 0 && Task_getChannelId(nullptr) == 0 &&
          Task_getRequesterId(nullptr) == 0 && Task_getAssigneeId(nullptr) == 0 &&
          Task_getParentId(nullptr) == 0 && Task_getPermissions(nullptr) == 0 &&
          Task_getState(nullptr) == 0);
    utf8 buf[256];
    bool cut = false;
    CHECK("task", "toString null self",
          Task_toString(nullptr, buf, sizeof(buf), &cut) && strcmp(buf, "nullptr") == 0);
    CHECK("task", "toString value",
          Task_toString(&root, buf, sizeof(buf), &cut) && strcmp(buf, "task 100 state=1") == 0);
    CHECK("task", "toStringStruct null self",
          Task_toStringStruct(nullptr, buf, sizeof(buf), &cut) && strcmp(buf, "nullptr") == 0);
    CHECK("task", "toStringStruct fields",
          Task_toStringStruct(&root, buf, sizeof(buf), &cut) &&
          strcmp(buf, "Task{id=100,channelId=7,requesterId=42,assigneeId=9,parentId=0,permissions=3,state=1}") == 0);
    CHECK("task", "toString truncates", !Task_toString(&root, buf, 4, &cut) && cut);
    CHECK("task", "toStringStruct truncates", !Task_toStringStruct(&root, buf, 10, &cut) && cut);
}

/** Runs all automated vex/harness suites or a single named suite passed as argv[1]. */
i32 main(i32 argc, utf8 **argv) {
    const SuiteEntry suites[] = {
        { "channel", suite_channel },
        { "message", suite_message },
        { "model_user", suite_model_user },
        { "support", suite_support },
        { "task", suite_task },
    };
    const utf8 *filter = argc > 1 ? argv[1] : nullptr;
    bytes ran = 0;

    for (bytes i = 0; i < sizeof(suites) / sizeof(suites[0]); i++) {
        if (filter && strcmp(filter, suites[i].name) != 0)
            continue;
        suites[i].run();
        ran++;
    }

    if (filter && ran == 0) {
        fprintf(stderr, "harness_test: unknown suite '%s'\n", filter);
        return 2;
    }
    if (g_failures != 0) {
        fprintf(stderr, "harness_test: %d FAILURES\n", g_failures);
        return 1;
    }
    puts("harness_test: PASS");
    return 0;
}
