#include "stdafx.h"
#include "TestLL.h"
#include "MaxDegGen.h"



// estimate model init matrix
int GetModelInitMtx( const vector<TStr>& CommandLineArgs )
{
	PNGraph G;
	if (GetModel(CommandLineArgs[GRAPHGEN], G) == -1)
		return -1;

	int Nodes = G->GetNodes();

	TKronMtx FitMtx;
	EstimateInitMtx(CommandLineArgs, G, FitMtx);
	// get log likelihood for obtained init matrix
	GetLogLike(G, FitMtx);
	return 0;
}

// get log likelihood for a given graph and a given matrix
void GetLogLike( const PNGraph& G, const TKronMtx& FitMtx, const double Nsp )
{
	TKroneckerLL KronLL(G, FitMtx, Nsp);
	KronLL.InitLL(G, FitMtx);
	KronLL.SetDegPerm();
	TFile << "TestLL GetLogLike(). Calculating log likelihood..." << endl;
	const int NSamples = 100000, WarmUp = 450000;
	int NId1 = 0, NId2 = 0;
	double AvgLL = 0;
	/*if (WarmUp > 0) {
	TFile << "TestLL approximate LL 1st step: " << KronLL.CalcApxGraphLL() << endl;
	for (int s = 0; s < WarmUp; s++) { KronLL.SampleNextPerm(NId1, NId2); }
	}		
	TFile << "TestLL approximate LL 2st step: " << KronLL.CalcApxGraphLL() << endl;
	for (int s = 0; s < NSamples; s++) {
	KronLL.SampleNextPerm(NId1, NId2); 
	AvgLL += KronLL.GetLL();
	}
	double LL = AvgLL / NSamples;
	TFile << "TestLL log-likelihood: " << LL << endl;*/
	TFltV GradV;
	AvgLL = 0;
	KronLL.SampleGradient(WarmUp, NSamples, AvgLL, GradV);
	TFile << "TestLL SampleGradient log-likelihood: " << AvgLL << endl;
	system("pause");
}
