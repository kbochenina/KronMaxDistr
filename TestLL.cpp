#include "stdafx.h"
#include "TestLL.h"
#include "MaxDegGen.h"
#include "KronGen.h"
#include "Scale.h"

TestLL::TestLL(const CmdArgs& A, TRnd& R): Args(A), Rnd(R) {
	Args.GetArgsArray(CommandLineArgs);
	TStr GraphGenLine;
	
	ErrCode = Args.GetLine(GRAPHGEN, GraphGenLine);
	ErrCode = GetModel(GraphGenLine, G);
	PlotDeg(G, "model", Args.GetPlt());
}

// estimate model init matrix
int TestLL::TestScaledMtx()
{
	if (ErrCode == -1) return -1;

	TKronMtx FitMtx;
	double LL = 0;
	// estimate init matrix and save permutation to a file perm.dat
	EstimateInitMtx(CommandLineArgs, G, FitMtx, LL, true);
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
	
	int Nodes = G->GetNodes();
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
void TestLL::GetLogLike( const PNGraph& G, const TKronMtx& FitMtx, const double Nsp )
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

// test the quality of approximation using different sizes of samples
// for each sample size N:
// 1. N random nodes are chosen from degree sequence of graph G
// 2. if Deg(Node) > N, Deg(Node) = N (excessive degrees are truncated)
// 3. Graph G` is a configuration model built by N nodes
// 4. Estimate Kronecker matrix on G`
// 5. Scale Kronecker matrix to fit number of edges in G
// 6. Reproduce graph G with the use of obtained initiator matrix
int TestLL::TestSamples( )
{
	if (ErrCode == -1) return -1;
	int MinSize = Args.GetMinSize();
	int SF = Args.GetSF();
	int Size = MinSize;
	int N = G->GetNodes();
	int E = G->GetEdges();
	int NIter = GetNIter(N);
	// estimate "true" FitMtx
	TKronMtx FitMtx;
	// estimate init matrix
	EstimateInitMtx(CommandLineArgs, G, FitMtx);
	// and get log-likelihood
	GetLogLike(G, FitMtx);

	// for each sample size N:
	for (; Size <= N; Size *= SF){
		TIntV DegV;
		// 1. N random nodes are chosen from degree sequence of graph G
		GetRndDegSeq(G, Size, Rnd, DegV);
		// 2. if Deg(Node) > N, Deg(Node) = N (excessive degrees are truncated)
		for (int i = 0; i < DegV.Len(); i++){
			if (DegV[i] > N)
				DegV[i] == N;
		}
		// 3. Graph G` is a configuration model built by N nodes
		PUNGraph GNew = TSnap::GenConfModel(DegV, Rnd);
		// 4. Estimate Kronecker matrix on G`
		TKronMtx FitMtx;
		// get directed graph from undirected
		PNGraph GNewDir = TSnap::ConvertGraph<PNGraph>(GNew);
		// estimate init matrix
		EstimateInitMtx(CommandLineArgs, GNewDir, FitMtx);
		// 5. Scale Kronecker matrix to fit number of edges in G
		ScaleFitMtxForEdges(FitMtx, NIter, E, TFile);
		// 6. Reproduce graph G with the use of obtained initiator matrix
		TFile << "After scaling (size " << Size << ")" << endl;
		GetLogLike(G, FitMtx);
		// get file name
		char SizeStr[10];
		_itoa(Size, SizeStr, 10);
		TStr NameTStr("kron_scaled"), SizeTStr(SizeStr);
		NameTStr += SizeTStr;
		int Seed = Rnd.GetUniDevInt(INT_MIN, INT_MAX);
		TVec<TFltPrV> KronDegIn, KronDegOut;
		// generate NKron Kronecker graphs
		GenKron(Args, FitMtx, KronDegIn, KronDegOut, Seed);
		Get
		/*if (PlotDegKron(Args, FitMtx, NameTStr) == -1)
			return -1;*/
	}
	return 0;
}
