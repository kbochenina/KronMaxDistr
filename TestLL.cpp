#include "stdafx.h"
#include "TestLL.h"
#include "MaxDegGen.h"
#include "KronGen.h"
#include "Scale.h"



// estimate model init matrix
int TestLL( const CmdArgs& Args )
{
	PNGraph G;

	vector<TStr> CommandLineArgs;
	Args.GetArgsArray(CommandLineArgs);
	
	TStr GraphGenLine, KronGenLine;
	if (Args.GetLine(GRAPHGEN, GraphGenLine) == -1
		|| Args.GetLine(KRONGEN, KronGenLine) == -1) 
		return -1;

	if (GetModel(GraphGenLine, G) == -1)
		return -1;

	int Nodes = G->GetNodes();
	PlotDeg(G, "model", Args.GetPlt());

	TKronMtx FitMtx;
	// estimate init matrix and save permutation to a file perm.dat
	EstimateInitMtx(CommandLineArgs, G, FitMtx, true);
	// get log likelihood for obtained init matrix
	GetLogLike(G, FitMtx);
	// scale for undirected if it is required
	bool IsDir = Args.IsDir();
	if (IsDir == false){
		ScaleFitMtxForUnDir(FitMtx, TFile);
		TFile << "After scaling for undirected graph... " << endl;
		FitMtx.Dump(TFile);
	}
	// get log likelihood for obtained init matrix
	GetLogLike(G, FitMtx);
	if (PlotDegKron(Args, FitMtx, "kron") == -1)
		return -1;
		
	int NIter = GetNIter(Nodes);
	int ErrCode = ScaleFitMtx(FitMtx, NIter, static_cast<int>(pow(2.00, NIter)), GetMaxDeg(G), Args.IsDirStr());
	if (ErrCode == -1)
		return -1;
	TFile << "After scaling..." << endl;
	GetLogLike(G, FitMtx);
	if (PlotDegKron(Args, FitMtx, "kron_scaled") == -1)
		return -1;

	return 0;
}

// get log likelihood for a given graph and a given matrix
void GetLogLike( const PNGraph& G, const TKronMtx& FitMtx, const double Nsp )
{
	TKroneckerLL KronLL(G, FitMtx, Nsp);
	KronLL.InitLL(G, FitMtx);
	//KronLL.LoadPerm();
	KronLL.SetDegPerm();
	TFile << "TestLL GetLogLike(). Calculating log likelihood..." << endl;
	const int NSamples = 1000000, WarmUp = 100000;
	int NId1 = 0, NId2 = 0;
	double AvgLL = 0;
	TFltV GradV;
	AvgLL = 0;
	KronLL.SampleGradient(WarmUp, NSamples, AvgLL, GradV);
	TFile << "TestLL SampleGradient log-likelihood: " << AvgLL << endl;
}
