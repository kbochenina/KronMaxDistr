#include "TestLL.h"

// estimate model init matrix
void GetModelInitMtx( const vector<TStr>& CommandLineArgs )
{
	PNGraph G;
	GetModel(CommandLineArgs[GRAPHGEN], G);
	TKronMtx FitMtx;
	EstimateInitMtx(CommandLineArgs, G, FitMtx);
}
