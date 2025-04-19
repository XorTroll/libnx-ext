
#pragma once
#include "cruiser/cruiser.h"

// https://github.com/syvb/switch-oss/blob/master/WKC_0.25.0/include/wkc/wkcdebugpeer.h

/**
@brief Report when an arrow function is used.
*/
CRUISER_API(void (*wkcSetReportArrowFunctionCallbackPeer)(void(*callback)(void)));

/**
@brief Report when 'this' is used in an arrow function.
*/
CRUISER_API(void (*wkcSetReportArrowFunctionThisCallbackPeer)(void(*callback)(void)));
