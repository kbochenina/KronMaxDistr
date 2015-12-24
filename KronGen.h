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