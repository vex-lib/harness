#include "vex.h"
#include "space/model_user.h"

;;DEFINITION
/**
 * DEFINITION: ModelUser
 *
 * ModelUser gives a model persona a stable server-local identity and mention
 * handle. It owns its copied name but no model execution or credential. The
 * modelId is host metadata; zero/invalid construction yields an empty value.
 */

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: ModelUser (space/model_user.c)
 * ============================================================================
 * STRUCT FIELDS (Mirroring space/model_user.h):
 * ----------------------------------------------------------------------------
 *   ModelUser {
 *     u64 id;                          // host-assigned identity
 *     utf8 name[MODEL_USER_NAME_CAP];       // copied mention handle
 *     u64 modelId;                     // selected model metadata
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Constructors:
 *   - ModelUser_0() / ModelUser_3(id, name, modelId)
 *   - ModelUser(...) / ModelUser_zero()
 *
 * Setters:
 *   - ModelUser_setName(self, name)
 *   - ModelUser_setModelId(self, modelId)
 *
 * Getters:
 *   - ModelUser_getId(self)
 *   - ModelUser_getName(self)
 *   - ModelUser_getModelId(self)
 *
 * Projections:
 *   - ModelUser_toString(self, dest, cap, outTruncated)
 *   - ModelUser_toStringStruct(self, dest, cap, outTruncated)
 *
 * Caller serializes mutation. Invalid setters preserve state; getters return
 * safe defaults on nullptr. Invalid construction returns the empty value.
 * ============================================================================
 */

;;INTENTION("IDs change only through construction, not setters that would corrupt server identity; per the Conflict Triage Law + Single Class Per File Law (Java Law)")

/**
 * Returns an empty identity with no registered model or handle.
 */
ModelUser ModelUser_0() {
    return (ModelUser) {0};
}

/**
 * Constructs a complete persona, copying the handle only after validation.
 */
ModelUser ModelUser_3(u64 id, string name, u64 modelId) {
    if (id == 0 || modelId == 0 || !Space_nameValid(name)) {
        THROW("model user: invalid identity");
        return ModelUser_0();
    }
    ModelUser result = ModelUser_0();
    result.id = id;
    result.modelId = modelId;
    memcpy(result.name, name, strlen(name) + 1);
    return result;
}

/**
 * Rejects an invalid name without changing the copied identity.
 */
;;GETTER
static inline utf8 *ModelUser_nameMut(ModelUser *self)
{
    return (*self).name;
}

;;SETTER
bool ModelUser_setName(ModelUser *self, string name) {
    if (self == nullptr || !Space_nameValid(name)) {
        THROW("model user name: invalid input");
        return false;
    }
    utf8 copy[MODEL_USER_NAME_CAP] = {0};
    memcpy(copy, name, strlen(name) + 1);
    memcpy(ModelUser_nameMut(self), copy, sizeof(copy));
    return true;
}

/**
 * Updates model metadata; zero is never a usable model binding.
 */
bool ModelUser_setModelId(ModelUser *self, u64 modelId) {
    if (self == nullptr || modelId == 0) {
        THROW("model user model: invalid input");
        return false;
    }
    (*self).modelId = modelId;
    return true;
}

/**
 * Returns the persona identity, or zero for nullptr.
 */
;;GETTER
u64 ModelUser_getId(const ModelUser *self) {
    if (self == nullptr)
        return 0;
    return (*self).id;
}

/**
 * Returns the borrowed mention handle, or nullptr for an absent persona.
 */
;;GETTER
string ModelUser_getName(const ModelUser *self) {
    if (self == nullptr)
        return nullptr;
    return (*self).name;
}

/**
 * Returns host model metadata, not provider capabilities; nullptr yields zero.
 */
;;GETTER
u64 ModelUser_getModelId(const ModelUser *self) {
    if (self == nullptr)
        return 0;
    return (*self).modelId;
}

/**
 * Formats the mention spelling; validated handles require no escaping.
 */
bool ModelUser_toString(const ModelUser *self, utf8 *dest, bytes cap, bool *outTruncated) {
    return self == nullptr ? Space_format(dest, cap, outTruncated, "nullptr") :
        Space_format(dest, cap, outTruncated, "@%s", ModelUser_getName(self));
}

/**
 * Formats own fields in declaration order without model/provider traversal.
 */
bool ModelUser_toStringStruct(const ModelUser *self, utf8 *dest, bytes cap, bool *outTruncated) {
    return self == nullptr ? Space_format(dest, cap, outTruncated, "nullptr") :
        Space_format(dest, cap, outTruncated, "ModelUser{id=%llu,name=\"%s\",modelId=%llu}",
            (u64) ModelUser_getId(self), ModelUser_getName(self), (u64) ModelUser_getModelId(self));
}
