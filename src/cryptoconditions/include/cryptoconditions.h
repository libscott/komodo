#include <cJSON.h>
#include <stdint.h>


#ifndef CRYPTOCONDITIONS_H
#define CRYPTOCONDITIONS_H


#ifdef __cplusplus
extern "C" {
#endif


struct CC;
struct CCType;


enum CCTypeId {
    CC_Anon = -1,
    CC_Preimage = 0,
    CC_Prefix = 1,
    CC_Threshold = 2,
    CC_Ed25519 = 4,
    CC_Secp256k1 = 5,
    CC_Eval = 15
};


/*
 * Evaliliary verification callback
 */
typedef int (*VerifyEval)(struct CC *cond, void *context);



/*
 * Crypto Condition
 */
typedef struct CC {
    struct CCType *type;
    union {
        // public key types
        struct { unsigned char *publicKey, *signature; };
        // preimage
        struct { unsigned char *preimage; uint16_t preimageLength; };
        // threshold
        struct { long threshold; uint8_t size; struct CC **subconditions; };
        // prefix
        struct { unsigned char *prefix; uint16_t prefixLength; struct CC *subcondition;
                 uint16_t maxMessageLength; };
        // eval
        struct { char method[64]; unsigned char *paramsBin; uint16_t paramsBinLength; };
        // anon
        struct { unsigned char fingerprint[32]; uint32_t subtypes; unsigned long cost; 
                 struct CCType *conditionType; };
    };
} CC;



/*
 * Crypto Condition Visitor
 */
typedef struct CCVisitor {
    int (*visit)(CC *cond, struct CCVisitor visitor);
    const unsigned char *msg;
    size_t msgLength;
    void *context;
} CCVisitor;


/*
 * Public methods
 */
int             cc_isFulfilled(const CC *cond);
int             cc_verify(const struct CC *cond, const unsigned char *msg, size_t msgLength,
                        int doHashMessage, const unsigned char *condBin, size_t condBinLength,
                        VerifyEval verifyEval, void *evalContext);
int             cc_visit(CC *cond, struct CCVisitor visitor);
int             cc_signTreeEd25519(CC *cond, const unsigned char *privateKey,
                                   const unsigned char *msg, uint16_t msgLength);
int             cc_signTreeSecp256k1Msg32(CC *cond, const unsigned char *privateKey, const unsigned char *msg32);
size_t          cc_conditionBinary(const CC *cond, unsigned char *buf);
size_t          cc_fulfillmentBinary(const CC *cond, unsigned char *buf, size_t bufLength);
static int      cc_secp256k1VerifyTreeMsg32(const CC *cond, const unsigned char *msg32);
struct CC*      cc_conditionFromJSON(cJSON *params, char *err);
struct CC*      cc_conditionFromJSONString(const char *json, char *err);
struct CC*      cc_readConditionBinary(const unsigned char *cond_bin, size_t cond_bin_len);
struct CC*      cc_readFulfillmentBinary(const unsigned char *ffill_bin, size_t ffill_bin_len);
struct CC*      cc_new(int typeId);
struct cJSON*   cc_conditionToJSON(const CC *cond);
char*           cc_conditionToJSONString(const CC *cond);
char*           cc_conditionUri(const CC *cond);
char*           cc_jsonRPC(char *request);
char*           cc_typeName(const CC *cond);
enum CCTypeId   cc_typeId(const CC *cond);
unsigned long   cc_getCost(const CC *cond);
uint32_t        cc_typeMask(const CC *cond);
int             cc_isAnon(const CC *cond);
void            cc_free(struct CC *cond);

#ifdef __cplusplus
}
#endif

#endif  /* CRYPTOCONDITIONS_H */