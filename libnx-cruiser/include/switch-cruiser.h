/**
 * @file switch-cruiser.h
 * @brief Central Switch header. Includes all others.
 * @copyright libnx Authors
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "cruiser/browserdll.h"
#include "cruiser/oss.h"
#include "cruiser/sym.h"

#include "cruiser/cruiser.h"
#include "cruiser/cruiser/wkc.h"
#include "cruiser/cruiser/jsc.h"

#ifdef __cplusplus
}
#endif

// C++-only headers

#ifdef __cplusplus
#include "cruiser/cruiser-cpp/cruiser.hpp"
#include "cruiser/cruiser-cpp/webkit.hpp"
#endif
