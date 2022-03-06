#pragma once

class OpcodeMgr
{
public:
	OpcodeMgr() = delete;
	OpcodeMgr(const OpcodeMgr&) = delete;
	void operator=(const OpcodeMgr&) = delete;

	static void RegisterCommands();
};

