class TestLL{
	CmdArgs Args;
	TRnd& Rnd;
	vector <TStr> CommandLineArgs;
	PNGraph G;
	int ErrCode;
	// get log likelihood for a given graph and a given matrix
	void GetLogLike(const PNGraph& G, const TKronMtx& FitMtx, const double Nsp = 1.0);

public:
	TestLL(const CmdArgs& A, TRnd& R);
	// test log-likelihood for scaled init matrix
	int TestScaledMtx();
	// test the quality of approximation using different sizes of samples
	// for each sample size N:
	// 1. N random nodes are chosen from degree sequence of graph G
	// 2. if Deg(Node) > N, Deg(Node) = N (excessive degrees are truncated)
	// 3. Graph G` is a configuration model built by N nodes
	// 4. Estimate Kronecker matrix on G`
	// 5. Scale Kronecker matrix to fit number of edges in G
	// 6. Reproduce graph G with the use of obtained initiator matrix
	int TestSamples();
};


