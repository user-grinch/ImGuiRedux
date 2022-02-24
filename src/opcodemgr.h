#pragma once

class OpcodeMgr
{
public:
	static inline bool bImGuiHooked;
	static inline size_t nFPS;
	static inline float frameHeight;

	OpcodeMgr() = delete;
	OpcodeMgr(const OpcodeMgr&) = delete;
	void operator=(const OpcodeMgr&) = delete;

	static void RegisterCommands();
};

