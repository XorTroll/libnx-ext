
#pragma once
#include "ext.h"

typedef union {
    struct {
        u64 app_id_be;
        u64 key_gen_be;
    };
    FsRightsId id;
} EsRightsId;
EXT_STATIC_ASSERT(sizeof(EsRightsId) == 0x10);

NX_CONSTEXPR u64 esGetRightsIdApplicationId(const EsRightsId *id) {
    return __builtin_bswap64(id->app_id_be);
}

NX_CONSTEXPR u8 esGetRightsIdKeyGeneration(const EsRightsId *id) {
    return (u8)(__builtin_bswap64(id->key_gen_be) & 0xFF);
}

Result esInitialize(void);
void esExit(void);

Result esImportTicket(const void *tik, size_t tik_size, const void *cert, size_t cert_size);
Result esDeleteTicket(const EsRightsId *rights_id);
u32 esCountCommonTicket();
u32 esCountPersonalizedTicket();
Result esListCommonTicket(u32 *out_written, EsRightsId *out_rights_id_buf, size_t out_rights_id_buf_size);
Result esListPersonalizedTicket(u32 *out_written, EsRightsId *out_rights_id_buf, size_t out_rights_id_buf_size);
