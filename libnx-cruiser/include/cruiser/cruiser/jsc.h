
#pragma once
#include "cruiser/cruiser.h"
#include "cruiser/cruiser/jsc-types.h"

CRUISER_API(JSGlobalContextRef(*JSGlobalContextCreate)(JSClassRef globalObjectClass));
CRUISER_API(void(*JSGlobalContextRelease)(JSGlobalContextRef ctx));

CRUISER_API(JSStringRef(*JSStringCreateWithUTF8CString)(const char* string));
CRUISER_API(void(*JSStringRelease)(JSStringRef string));
CRUISER_API(void*(*JSEvaluateScript)(JSGlobalContextRef ctx, JSStringRef script, JSObjectRef thisObject, JSStringRef sourceURL, int startingLineNumber, JSValueRef* exception));

CRUISER_API(double(*JSValueToNumber)(JSContextRef ctx, JSValueRef value, JSValueRef* exception));
