#pragma once

class OpcodeMgr
{
public:
	static inline bool bImGuiHooked;

	OpcodeMgr() = delete;
	OpcodeMgr(const OpcodeMgr&) = delete;
	void operator=(const OpcodeMgr&) = delete;

	static void RegisterCommands();
};

