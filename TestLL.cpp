#include "stdafx.h"
#include "TestLL.h"
#include "MaxDegGen.h"
#include "KronGen.h"



// estimate model init matrix
int GetModelInitMtx( const vector<TStr>& CommandLineArgs )
{
	PNGraph G;
	if (GetModel(CommandLineArgs[GRAPHGEN], G) == -1)
		return -1;

	int Nodes = G->GetNodes();

	TKronMtx FitMtx;
	// estimate init matrix and save permutation to a file perm.dat
	EstimateInitMtx(CommandLineArgs, G, FitMtx, true);
	// get log likelihood for obtained init matrix
	GetLogLike(G, FitMtx);
	return 0;
}

// get log likelihood for a given graph and a given matrix
void GetLogLike( const PNGraph& G, const TKronMtx& FitMtx, const double Nsp )
{
	TKroneckerLL KronLL(G, FitMtx, Nsp);
	KronLL.InitLL(G, FitMtx);
	KronLL.LoadPerm();
	TFile << "TestLL GetLogLike(). Calculating log likelihood..." << endl;
	const int NSamples = 10000, WarmUp = 100000;
	int NId1 = 0, NId2 = 0;
	double AvgLL = 0;
	TFltV GradV;
	AvgLL = 0;
	KronLL.SampleGradient(WarmUp, NSamples, AvgLL, GradV);
	TFile << "TestLL SampleGradient log-likelihood: " << AvgLL << endl;
	system("pause");
}
