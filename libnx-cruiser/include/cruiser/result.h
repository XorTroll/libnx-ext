// ...
#pragma once
#include "ext.h"

enum {
    Module_Cruiser = 350,
};

enum {
    CruiserResult_FailedToInitialize = 1,
    CruiserResult_NotInitialized,
    CruiserResult_ModuleLimitReached,
    CruiserResult_InvalidArgument,
    CruiserResult_InvalidMainAslrBase,
    CruiserResult_InvalidInputNro,
    CruiserResult_InvalidModuleState,
    CruiserResult_InvalidRelaSize,
    CruiserResult_InvalidRelSize,
    CruiserResult_InvalidRelrSize,
    CruiserResult_InvalidSymSize,
    CruiserResult_InvalidPltRel,
    CruiserResult_SymbolNotFound,
    CruiserResult_InvalidBrowserDllNro,
    CruiserResult_CalledStubbedFakeSymbol,
    CruiserResult_Lz4DecompressionFailed,
};
