#include "stdafx.h"
#include "InOut.h"

bool CmdArgs::IsDir() const {
	Env = TEnv(CommandLineArgs[KRONGEN], TNotify::NullNotify);
	TStr IsDir = Env.GetIfArgPrefixStr("-isdir:", "false", "Produce directed graph (true, false)");
	if (IsDir == "true")
		return true;
	return false;
}

TStr CmdArgs::IsDirStr() const
{
	Env = TEnv(CommandLineArgs[KRONGEN], TNotify::NullNotify);
	TStr IsDir = Env.GetIfArgPrefixStr("-isdir:", "false", "Produce directed graph (true, false)");
	return IsDir;
}

int CmdArgs::GetLine( const int Index, TStr& Line ) const
{
	if (Index < 0 || Index > CommandLineArgs.size()-1)
		return Error("ParseArgs::GetLine", "Wrong line index");
	Line = CommandLineArgs[Index];
	return 0;
}

void CmdArgs::GetArgsArray( std::vector<TStr>& CLA ) const
{
	CLA.clear();
	for (size_t i = 0; i < CommandLineArgs.size(); ++i){
		CLA.push_back(CommandLineArgs[i]);
	}
}

TStr CmdArgs::GetPlt() const
{
	Env = TEnv(CommandLineArgs[KRONTEST], TNotify::NullNotify);
	TStr Plt = Env.GetIfArgPrefixStr("-plt:", "all", "What to plot (cum, noncum, all)");
	return Plt;
}

CmdArgs::CmdArgs( const std::vector<TStr>& CLA )
{
	for (size_t i = 0; i < CLA.size(); ++i){
		CommandLineArgs.push_back(CLA[i]);
	}
}
