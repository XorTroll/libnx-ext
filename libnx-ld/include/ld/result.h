// ...
#pragma once
#include "ext.h"

enum {
    Module_Ld = 350,
};

enum {
    LdResult_FailedToInitialize = 1,
    LdResult_NotInitialized,
    LdResult_ModuleLimitReached,
    LdResult_InvalidArgument,
    LdResult_InvalidMainAslrBase,
    LdResult_InvalidInputNro,
    LdResult_InvalidModuleState,
    LdResult_InvalidRelaSize,
    LdResult_InvalidRelSize,
    LdResult_InvalidRelrSize,
    LdResult_InvalidSymSize,
    LdResult_InvalidPltRel,
    LdResult_SymbolNotFound,
    LdResult_UnresolvedSymbol,
};
