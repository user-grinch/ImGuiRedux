#pragma once
#include "pch.h"
#ifdef RUNTIME_CLEO
#include "../include/cleo/CLEO.h"
#define RTN_CONTINUE CLEO::OR_CONTINUE
#define RTN_TYPE CLEO::OpcodeResult
#define RUNTIME_API WINAPI
#define RUNTIME_CONTEXT CLEO::CRunningScript*
#define CMD_DEF CLEO::_pOpcodeHandler
#define INT int
#else
#include "../include/cleo/cleo_redux_sdk.h"
#define RTN_CONTINUE HandlerResult::CONTINUE
#define RTN_TYPE HandlerResult 
#define RUNTIME_API
#define RUNTIME_CONTEXT Context
#define CMD_DEF CommandHandler
#define INT isize
#endif

#define RUNTIME_STR_LEN 128


static void wOnGameLoadEvent(void* func) {
#ifdef RUNTIME_CLEO
    CLEO::CLEO_RegisterCallback(CLEO::eCallbackId::GameBegin, func);
#else
    OnRuntimeInit((OnRuntimeInitCallback)func);
#endif
}

static void wGetStringParam(RUNTIME_CONTEXT ctx, char* label, unsigned char length) {
#ifdef RUNTIME_CLEO
    CLEO_ReadStringOpcodeParam(ctx, label, length);
#else
    GetStringParam(ctx, label, length);
#endif
}

/*
	Appending BeginFrame uniqueID to widget names
	This ensures scripts won't clash with each other
*/
static void wGetStringWithFrame(RUNTIME_CONTEXT ctx, char* label, unsigned char length) {
    wGetStringParam(ctx, label, length);
    strcat(label, "##");
    strcat(label, ScriptExData::GetCurrentScript().c_str());
}

static void wSetIntParam(RUNTIME_CONTEXT ctx, INT value) {
#ifdef RUNTIME_CLEO
    CLEO_SetIntOpcodeParam(reinterpret_cast<CLEO::CRunningScript*>(ctx), value);
#else
    SetIntParam(ctx, value);
#endif
}

static void wSetFloatParam(RUNTIME_CONTEXT ctx, float value) {
#ifdef RUNTIME_CLEO
    CLEO_SetFloatOpcodeParam(reinterpret_cast<CLEO::CRunningScript*>(ctx), value);
#else
    SetFloatParam(ctx, value);
#endif
}

static INT wGetIntParam(RUNTIME_CONTEXT ctx) {
#ifdef RUNTIME_CLEO
    return CLEO_GetIntOpcodeParam(reinterpret_cast<CLEO::CRunningScript*>(ctx));
#else
    return GetIntParam(ctx);
#endif
}

static bool wGetBoolParam(RUNTIME_CONTEXT ctx) {
#ifdef RUNTIME_CLEO
    return static_cast<bool>(CLEO_GetIntOpcodeParam(reinterpret_cast<CLEO::CRunningScript*>(ctx)));
#else
    return static_cast<bool>(GetIntParam(ctx));
#endif
}

static float wGetFloatParam(RUNTIME_CONTEXT ctx) {
#ifdef RUNTIME_CLEO
    return CLEO_GetFloatOpcodeParam(reinterpret_cast<CLEO::CRunningScript*>(ctx));
#else
    return GetFloatParam(ctx);
#endif
    return -1.0f;
}

static void wUpdateCompareFlag(RUNTIME_CONTEXT ctx, bool flag) {
#ifdef RUNTIME_CLEO
    typedef unsigned __int16 (__thiscall* _type)(RUNTIME_CONTEXT, bool);
    ((_type)0x4859D0)(ctx, flag); // CRunningScript::UpdateCompareFlag();
#else
    UpdateCompareFlag(ctx, flag);
#endif
}

static void wResolvePath(RUNTIME_CONTEXT ctx, const char* src, char* dest, size_t maxLength) {
#ifdef RUNTIME_CLEO
    strcpy(dest, src);
    CLEO_ResolvePath(ctx, dest, maxLength);
#else
    ResolvePath(src, dest);
#endif
}

static void wRegisterCommand(const char *name, CMD_DEF handler) {
#ifdef RUNTIME_CLEO
    static size_t opcode = 0x2200;
    CLEO_RegisterOpcode(opcode, handler);
    opcode++;
#else
    RegisterCommand(name, handler);
#endif
}

static void wSetStringParam(RUNTIME_CONTEXT ctx, const char* buf) {
#ifdef RUNTIME_CLEO
    CLEO_WriteStringOpcodeParam(reinterpret_cast<CLEO::CRunningScript*>(ctx), buf);
#else
    SetStringParam(ctx, buf);
#endif
}