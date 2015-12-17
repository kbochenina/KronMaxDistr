// KronTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "InOut.h"
#include "KronGen.h"
#include "MaxDegGen.h"
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
    ReadParameters(SettingsFNm, commandLineArgs);
    // file name with statistics
    Env = TEnv(commandLineArgs[KRONTEST], TNotify::NullNotify);
    const TStr StatFile = Env.GetIfArgPrefixStr("-ot:", "stat.tab", "Name of output file with statistics");
    // TFile is global variable
    TFile = OpenFile(StatFile.CStr());
    //KroneckerBySample(commandLineArgs);
    // MaxDegGen(commandLineArgs); 
    //TestRealGraph(commandLineArgs);

    TFile.close();
    return 0;
}

