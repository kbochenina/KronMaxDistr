// estimate model init matrix
int GetModelInitMtx(const vector<TStr>& CommandLineArgs);
// get log likelihood for a given graph and a given matrix
void GetLogLike(const PNGraph& G, const TKronMtx& FitMtx, const double Nsp = 1.0);
