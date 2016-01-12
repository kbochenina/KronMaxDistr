
#ifndef KRONGEN_H
#define KRONGEN_H

static double DEFLL = 0;

// generates Kronecker model using sample of network
int KroneckerBySample(vector<TStr> commandLineArgs);
// generates graph using arguments from Args
int GetModel(const TStr& Args, PNGraph& G);
// read or get random mtx
bool GetMtx(const TStr& MtxArgs, TKronMtx& FitMtxModel);
void GenNewMtx(const PNGraph& model, const TStr& args, TKronMtx& FitMtx);
// get approximate NIter for a particular size
int GetNIter(const int Size);
int GetGraphs(const vector <TStr>& Parameters, const TStr& ModelGen, const TStr&ModelPlt);
// generate new initiator matrix
int InitKronecker(const TStr args, const PNGraph &GD, TKronMtx& FitMtx, double& LL = DEFLL, bool SavePerm = false);
void GenKron(const CmdArgs& Args, const TKronMtx& FitMtx, TFltPrV& KronDegAvgIn, TFltPrV& KronDegAvgOut);
// the overload of GenKron returning array of degree sequences for NKron graphs
void GenKron(const CmdArgs& Args, const TKronMtx& FitMtx, TVec<TFltPrV>& KronDegIn, TVec<TFltPrV>& KronDegOut, TRnd& Rnd );

#endif