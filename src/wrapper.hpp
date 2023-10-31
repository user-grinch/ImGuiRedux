#pragma once
#include "../cleo/cleo_redux_sdk.h"
#include "../cleo/cleo_sdk.h"

static void wGetStringParam(Context ctx, char* label, unsigned char length) {
    if constexpr (gRunTime == eRunTime::Cleo) {
        CLEO_ReadStringOpcodeParam(reinterpret_cast<CScriptThread*>(ctx), label, length);
    } else {
        GetStringParam(ctx, label, length);
    } 
}

/*
	Appending BeginFrame uniqueID to widget names
	This ensures scripts won't clash with each other
*/
static void wGetStringWithFrame(Context ctx, char* label, unsigned char length) {
    wGetStringParam(ctx, label, length);
    strcat(label, "##");
    strcat(label, ScriptExData::GetCurrentScript().c_str());
}

static void wSetIntParam(Context ctx, isize value) {
    if constexpr (gRunTime == eRunTime::Cleo) {
        CLEO_SetIntOpcodeParam(reinterpret_cast<CScriptThread*>(ctx), value);
    } else {
        SetIntParam(ctx, value);
    } 
}

static void wSetFloatParam(Context ctx, float value) {
    if constexpr (gRunTime == eRunTime::Cleo) {
        CLEO_SetFloatOpcodeParam(reinterpret_cast<CScriptThread*>(ctx), value);
    } else {
        SetFloatParam(ctx, value);
    } 
}

static isize wGetIntParam(Context ctx) {
    if constexpr (gRunTime == eRunTime::Cleo) {
        return CLEO_GetIntOpcodeParam(reinterpret_cast<CScriptThread*>(ctx));
    } else {
        return GetIntParam(ctx);
    } 
    return -1;
}

static float wGetFloatParam(Context ctx) {
    if constexpr (gRunTime == eRunTime::Cleo) {
        return CLEO_GetFloatOpcodeParam(reinterpret_cast<CScriptThread*>(ctx));
    } else {
        return GetFloatParam(ctx);
    } 
    return -1.0f;
}

static void wUpdateCompareFlag(Context ctx, bool flag) {
    if constexpr (gRunTime == eRunTime::Cleo) {
        ((void(*)(Context, bool))0x4859D0)(ctx, flag); // CRunningScript::UpdateCompareFlag();
    } else {
        UpdateCompareFlag(ctx, flag);
    }
}

static void wRegisterCommand(const char *name, CommandHandler handler) {
    static size_t startOpcode = 0x2100;
    if constexpr (gRunTime == eRunTime::Cleo) {
        CLEO_RegisterOpcode(startOpcode, reinterpret_cast<_pOpcodeHandler>(handler));
        startOpcode++;
    } else {
        RegisterCommand(name, handler);
    }
}

static void wSetStringParam(Context ctx, const char* buf) {
    if constexpr (gRunTime == eRunTime::Cleo) {
        CLEO_WriteStringOpcodeParam(reinterpret_cast<CScriptThread*>(ctx), buf);
    } else {
        SetStringParam(ctx, buf);
    }
}