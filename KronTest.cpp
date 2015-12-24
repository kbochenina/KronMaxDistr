// KronTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "InOut.h"
#include "KronGen.h"
#include "MaxDegGen.h"
#include "TestLL.h"
#include <boost/random/mersenne_twister.hpp>

ofstream TFile;

int _tmain(int argc, _TCHAR* argv[])
{
	boost::mt19937 g;
    Env = TEnv(argc, argv, TNotify::StdNotify);
	Env.PrepArgs(TStr::Fmt("KronTest. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
    const TStr SettingsFNm = Env.GetIfArgPrefixStr("-s:", "parameters.txt", "Settings filename");
    std::vector<TStr> commandLineArgs;
    // read command line arguments for all generators
    if (ReadParameters(SettingsFNm, commandLineArgs) == -1){
		TFile.close();
		return -1;
	}
    // file name with statistics
    Env = TEnv(commandLineArgs[KRONTEST], TNotify::NullNotify);
    const TStr StatFile = Env.GetIfArgPrefixStr("-ot:", "stat.tab", "Name of output file with statistics");
    // TFile is global variable
	bool IsError = false;
    TFile = OpenFile(StatFile.CStr(), IsError);
	if (IsError)
		return -1;
    //if (!KroneckerBySample(commandLineArgs)) {
	// TFile.close();
	// return -1;
	//}
    // if (MaxDegGen(commandLineArgs) == -1)
	//		return -1; 
    //TestRealGraph(commandLineArgs);
	if (GetModelInitMtx(commandLineArgs) == -1){
		TFile.close();
		return -1;
	}
    return 0;
}

