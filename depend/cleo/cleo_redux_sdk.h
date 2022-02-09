#pragma once

#define STR_MAX_LEN 128

enum HandlerResult: char
{
	HR_CONTINUE = 0,
	HR_TERMINATE = 1,
	HR_ERROR = -1
};

enum HostId: char
{
	re3 = 1,
	reVC = 2,
	GTA3 = 3,
	VC = 4,
	SA = 5,
	GTA3_UNREAL = 6,
	VC_UNREAL = 7,
	SA_UNREAL = 8
};

typedef void* Context;
typedef HandlerResult (*CommandHandler)(Context);

extern "C" {
	long GetSDKVersion();
	HostId GetHostId();
	void GetCLEOFolder(char* buf);
	
	void Log(const char* text);
	void RegisterCommand(const char* name, CommandHandler handler, const char* permission = nullptr);

	long GetIntParam(Context ctx);
	float GetFloatParam(Context ctx);
	void GetStringParam(Context ctx, char* buf, unsigned char len);
	void SetIntParam(Context ctx, long value);
	void SetFloatParam(Context ctx, float value);
	void SetStringParam(Context ctx, char* buf, unsigned char len);
	void UpdateCompareFlag(Context ctx, bool result);
}
