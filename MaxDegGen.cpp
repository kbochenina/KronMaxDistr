#include "stdafx.h"
#include "stdlib.h"
#include "MaxDegGen.h"

int GetMaxDeg(const PNGraph& G, bool IsIn){
    TFltPrV Deg;
    if (IsIn)
        TSnap::GetInDegCnt(G, Deg);
    else
        TSnap::GetOutDegCnt(G, Deg);
    //PrintTFltPrV(Deg);
    return static_cast<int>(Deg[Deg.Len()-1].GetVal1());
}

int GetMaxDeg(const PNGraph& G){
	TFltPrV Deg;
	TSnap::GetOutDegCnt(G, Deg);
	//PrintTFltPrV(Deg);
	return static_cast<int>(Deg[Deg.Len()-1].GetVal1());
}

int GetMaxDegFromGEV( double Scale, double Location, double Shape, double mult = 1 )
{
    double r = ((double) rand() / (RAND_MAX));
    int Deg = 0;
    if (Shape > 0)
        Deg = static_cast<int>(ceil(GEVGetMinX(Scale, Location, Shape)));
    int MaxDeg = INT_MAX;
    if (Shape < 0)
        MaxDeg = static_cast<int>(floor(GEVGetMaxX(Location, Scale, Shape)));
	// cout << "Deg: " << Deg << " MaxDeg: " << MaxDeg << endl;
    cout << "Deg: " << Deg << " Cdf: " << GevCDF(Scale, Location, Shape, Deg) << " r = " << r << endl;
    while (GevCDF(Scale, Location, Shape, Deg, mult) < r && Deg <= MaxDeg){
        //cout << "Deg: " << Deg << " Cdf: " << GevCDF(Scale, Location, Shape, Deg, mult) << " r = " << r << " mult = " << mult << endl;
        ++Deg;
    }
    return Deg;
}

// truncated distribution 
int GevMaxDegFromGEVTrunc( double Scale, double Location, double Shape, double Mult, double A, double B ){
	double r = ((double) rand() / (RAND_MAX));
	int Deg = 0;
	if (Shape > 0)
		Deg = static_cast<int>(ceil(GEVGetMinX(Scale, Location, Shape)));
	int MaxDeg = INT_MAX;
	if (Shape < 0)
		MaxDeg = static_cast<int>(floor(GEVGetMaxX(Location, Scale, Shape)));

	bool Flag = false;
	while (!Flag){
		// GevTrunc = [F(x)-F(a)] / [F(b)-F(a)]
		/*double F_A = GevCDF(Scale, Location, Shape, A, Mult);
		double F_B = GevCDF(Scale, Location, Shape, B, Mult);
		double GevTrunc = (GevCDF(Scale, Location, Shape, Deg, Mult) - F_A) / (F_B - F_A);*/
		double F_A = GevCDF(Scale, Location, Shape, A, 1);
		double F_B = GevCDF(Scale, Location, Shape, B, 1);
		double GevTrunc = (GevCDF(Scale, Location, Shape, Deg, 1) - F_A) / (F_B - F_A);
		GevTrunc = pow(GevTrunc, Mult);
		//double GevTrunc = Deg <= B ? GevCDF(Scale, Location, Shape, Deg, Mult) : 1;
		if (GevTrunc >= r || Deg > MaxDeg)
			Flag = true;
		++Deg;
	}
	return Deg;
}

void GetRndSubGraph(const PNGraph &G, int Size, PNGraph& SubGraph, TRnd& Rnd){
	SubGraph = TNGraph::New();
	TIntV NIdV;
	G->GetNIdV(NIdV);
	NIdV.Shuffle(Rnd);
	NIdV.Del(Size, NIdV.Len()-1);
	for (int i = 0; i < Size; ++i)
		SubGraph->AddNode(NIdV[i]);
	for (TNGraph::TEdgeI EI = G->BegEI(); EI != G->EndEI(); EI++){
		int Src = EI.GetSrcNId(), 
			Dst = EI.GetDstNId();
		if (SubGraph->IsNode(Src) && SubGraph->IsNode(Dst))
			SubGraph->AddEdge(Src, Dst);
	}
}

// GenType = 1 - random subgraph with truncation
// GenType = 2 - random subgraph without truncation
// GenType = 3 - random BFS without truncation
// GenType = 3 - random BFS with truncation
// NOTE: when GenType = 2, returns subgraph with truncation and MaxDeg without truncation!
// when GenType = 3, return subgraphs with nodes and without edges and MaxDeg without truncation
// when GenType = 4, return subgraph with truncation and MaxDeg with truncation
void GetSubGraph( int Size, int GenType, const PNGraph& G, TRnd& Rnd, vector<PNGraph>& SubGraphs, int& MaxDeg )
{
	/*TFltPrV DegSeq;
	TSnap::GetDegCnt(G, DegSeq);
	PrintTFltPrV(DegSeq);*/
	PNGraph SubGraph;
	int GNodes = G->GetNodes();
	if (GenType == 1 || GenType == 2)
		GetRndSubGraph(G, Size, SubGraph, Rnd);
		//SubGraph = TSnap::GetRndSubGraph(G, Size);
	if (GenType == 1){
		MaxDeg = GetMaxDeg(SubGraph);
		/*TFltPrV DegSeq;
		TSnap::GetDegCnt(SubGraph, DegSeq);
		PrintTFltPrV(DegSeq);*/
	}
	else if (GenType == 2){
		TIntV NIdVec;
		SubGraph->GetNIdV(NIdVec);
		int MaxDegSubGraph = 0;
		for (int i = 0; i < NIdVec.Len(); ++i){
			int SubGraphID = NIdVec[i];
			/*cout << "SubGraph id: " << SubGraphID << " SubGraph degree: " 
				<< SubGraph->GetNI(SubGraphID).GetDeg() << " Graph Deg: " << G->GetNI(SubGraphID).GetDeg() << endl;*/
			int GDeg = G->GetNI(SubGraphID).GetOutDeg();
			if (GDeg > MaxDegSubGraph)
				MaxDegSubGraph = GDeg;
		}
		MaxDeg = MaxDegSubGraph;
	}
	else if (GenType == 3 || GenType == 4){
		int NodesAdded = 0;
		TIntV NodesToAdd;
		SubGraph = TNGraph::New();
		while (SubGraph->GetNodes() != Size){
			int NID = 0;
			if (NodesToAdd.Len() == 0){
				NID = G->GetRndNId(Rnd);
				// cout << "NID: " << NID << endl;
			}
			else {
				/*for (int i = 0; i < NodesToAdd.Len(); i++)
					cout << NodesToAdd[i] << " ";
				cout << endl;*/
				NID = NodesToAdd[0];
				NodesToAdd.Del(0);
			}
			TIntV In, Out; 
			TNGraph::TNodeI NodeIt = G->GetNI(NID);
			int InDeg = NodeIt.GetInDeg(), OutDeg = NodeIt.GetOutDeg();
			for (int i = 0; i < InDeg; ++i){
				int InNID = NodeIt.GetInNId(i);
				In.Add(InNID);
				if (NodesToAdd.SearchForw(InNID) == -1)
					NodesToAdd.Add(InNID);
			}
			for (int i = 0; i < OutDeg; ++i){
				int OutNID = NodeIt.GetOutNId(i);
				Out.Add(OutNID);
				if (NodesToAdd.SearchForw(OutNID) == -1)
					NodesToAdd.Add(OutNID);
			}
			if (!SubGraph->IsNode(NID)){ 
				SubGraph->AddNode(NID);
				/*if (InDeg + OutDeg == 2918)
					cout << "(" << SubGraph->GetNodes() << ") "<< "Node " << NID << " was added" ;*/
			}
			if (GenType == 3){
				int Deg = InDeg + OutDeg;
				if (MaxDeg < Deg)
				MaxDeg = Deg;
			}
		}
	}
	if (GenType == 4){
		TIntV NIDV;
		SubGraph->GetNIdV(NIDV);
		for (int i = 0; i < NIDV.Len(); ++i){
			// get node
			int NID = NIDV[i];
			TNGraph::TNodeI NodeIt = G->GetNI(NID);
			int InDeg = NodeIt.GetInDeg(), OutDeg = NodeIt.GetOutDeg();
			for (int i = 0; i < InDeg; ++i){
				int InNID = NodeIt.GetInNId(i);
				if (SubGraph->IsNode(InNID) && !SubGraph->IsEdge(InNID, NID))
					SubGraph->AddEdge(InNID, NID);
			}
			for (int i = 0; i < OutDeg; ++i){
				int OutNID = NodeIt.GetOutNId(i);
				if (SubGraph->IsNode(OutNID) && !SubGraph->IsEdge(NID, OutNID))
					SubGraph->AddEdge(NID, OutNID);
			}
		}
		MaxDeg = GetMaxDeg(SubGraph);
	}
	SubGraphs.push_back(SubGraph);
}

// generates:
// TESTGRAPH line in configuration file
// read graph with the name -fn with size Size
// estimate matrix according to the random sample of size -minsize
// maximum size is calculated as MaxSize = max(MinSize * K * ScaleFactor) <= Size
// ScaleFactor is -sf parameter
// -nsamples is the number of samples of a particular size
// [MinSize; MinSize * ScaleFactor; ...; MaxSize; Size] are the sizes of samples
// estimates GEV according to NSamples of MinSize
// scales initiator matrix to tune number of edges (!)
// CurrGEV = pow(BaseGEV, K)
// scales maximum degree to match CurrGEV
// generates samples of Kronecker graphs
// GenType = 1 - random subgraph with truncation
// GenType = 2 - random subgraph without truncation
// GetType = 3 - random subnetwork without truncation
void TestRealGraph(const vector<TStr>& CommandLineArgs){

    Env = TEnv(CommandLineArgs[TESTGRAPH], TNotify::NullNotify);
    const TStr InFNm = Env.GetIfArgPrefixStr("-fn:", "as20graph.txt", "Name of file with graph");
	const TStr AllGev = Env.GetIfArgPrefixStr("-allgev:", "false", "Estimate GEV parameters for all sizes");
    const TInt MinSize = Env.GetIfArgPrefixInt("-minsize:", 100, "Minimum sample size");
	const TInt SF = Env.GetIfArgPrefixInt("-sf:", 1, "How many times size will increase at each step?");
	const TInt NSamples = Env.GetIfArgPrefixInt("-nsamples:", 10, "Number of samples");
	const TInt GenType = Env.GetIfArgPrefixInt("-gentype:", 3, "How the subgraph will be chosen");
	const TFlt TruncCoeff = Env.GetIfArgPrefixFlt("-trunc:", 1.0 , "Truncation coefficient");

	

	TRnd Rnd(static_cast<int>(time(NULL)));
    // read PNGraph from file
    PNGraph G = TSnap::LoadEdgeList<PNGraph>(InFNm, 0, 1);
	int GNodes = G->GetNodes();
	cout << "Nodes count in G: " << GNodes << endl;
	vector<vector<PNGraph>> SubGraphsDiffSizes;
	vector<vector<int>> MaxDegModelDiffSizes;
	vector<vector<int>> MaxDegGEVDiffSizes;
	
	int Size = MinSize, RealSize = G->GetNodes();

	Env = TEnv(CommandLineArgs[MAXDEGGEN], TNotify::NullNotify);
	// output filename
	const TStr MaxDegFile = Env.GetIfArgPrefixStr("-fn:", "maxdeg.tab", "Name of output file with statistics");
	ofstream Out(MaxDegFile.CStr());
	WriteHeader(Out, CommandLineArgs[GRAPHGEN], NSamples, static_cast<int>(log(RealSize)/log(10.0)), SF, true);

	TKronMtx FitMtx;
	double Scale = 0, Location = 0, Shape = 0;

	int GMaxDeg = GetMaxDeg(G);

	double A = 0, B = 0;

	while (Size <= RealSize){
		// get NSamples samples of minimum size
		vector<PNGraph> SubGraphs;
		vector<int> MaxDegModel;
		for (int i = 0; i < NSamples; ++i){
			if (i != 0 && i % 10 == 0) 
				cout << "Size = " << Size <<  " NSample = " << i << endl;
			int MaxDeg = 0;
			GetSubGraph(Size, GenType, G, Rnd, SubGraphs, MaxDeg);
			MaxDegModel.push_back(MaxDeg);
			// estimate initiator matrix for the first sample
			// only for truncated versions
			/*if (i == 0 && Size == MinSize && GenType == 1 || GenType == 4)
				EstimateInitMtx(CommandLineArgs, G, FitMtx);*/
		}
		if (Size == MinSize || AllGev == "true"){
			// estimate GEV parameters from NSamples samples
			GetGevParams(CommandLineArgs[MAXDEGGEN], MaxDegModel, MaxDegModel, Scale, Location, Shape);
			Out << "Size: " << Size << " Shape = " << Shape << " Location = " << Location << " Scale = " << Scale << endl;
			for (int i = 0; i < MaxDegModel.size(); ++i)
				if (MaxDegModel[i] > B)
					B = MaxDegModel[i];
		}

		B = B * TruncCoeff;
		cout << "Truncation coefficient: " << TruncCoeff << endl;
		cout << "Maximum degree: " << B << endl;
		cout << "Shape = " << Shape << " Location = " << Location << " Scale = " << Scale << endl;

		int Mult = Size / MinSize;
		cout << "Mult = " << Mult << endl;
		//vector<int> MaxDegGEV;
		//// Get NSamples values from GEV distribution
		//for (int i = 0; i < NSamples; i++){
		//	int MaxDeg = 0;
		//	//if (Size == MinSize)
		//	if (TruncCoeff == 0.0){
		//		//cout << "Without truncation: " << endl;
		//		MaxDeg = GetMaxDegFromGEV(Scale, Location, Shape, Mult);
		//	}
		//	else
		//		MaxDeg = GevMaxDegFromGEVTrunc(Scale, Location, Shape, Mult, A, B);
		//	//else
		//	//	MaxDeg = GetMaxDegFromGEV(Scale, Location, Shape, SF);
		//	MaxDegGEV.push_back(MaxDeg);
		//	if (i != 0 && i % 10 == 0) 
		//		cout << "Size = " << Size <<  " NSample = " << i << endl;
		//}
		////GetGevParams(CommandLineArgs[MAXDEGGEN], MaxDegGEV, MaxDegGEV, Scale, Location, Shape);
		//MaxDegGEVDiffSizes.push_back(MaxDegGEV);
		SubGraphsDiffSizes.push_back(SubGraphs);
		MaxDegModelDiffSizes.push_back(MaxDegModel);
		Size *= SF;
	}

	int i = 0;
	for (int Size = MinSize; Size <= RealSize; Size *= SF){
		WriteMaxDegSeq(Out, "Model", Size, MaxDegModelDiffSizes[i]);
		//WriteMaxDegSeq(Out, "GEV", Size, MaxDegGEVDiffSizes[i++]);
		++i;
	}
	
	Out << "Model maximum degree: " << GMaxDeg << endl;
	Out.close();
	// estimate Kronecker graph by first sample
	//TKronMtx FitMtx;
	//EstimateInitMtx(CommandLineArgs, SubGraphs[0], FitMtx);
	//// estimate GEV by samples
	//// estimate GEV parameters from N samples
	//double Scale = 0, Location = 0, Shape = 0;
	//GetGevParams(CommandLineArgs[MAXDEGGEN], MaxDegModel, MaxDegModel, Scale, Location, Shape);

}

// generates:
// N samples with size according to -n parameter of GRAPHGEN
// a set of Kronecker graphs
// minimum size is calculated as MinSize = pow(2, NI)
// where NI (a number of iterations) is the value of -i parameter of KRONGEN 
// maximum size is calculated as MaxSize = MinSize * ScaleFactor
// where ScaleFactor is the value of -sf parameter of KRONGEN
// for each size of Kronecker graph N samples will be calculated
// writes:
// maximum degrees of generated graphs to a file 
// with a given filename (-fn parameter of MAXDEGGEN)
// default filename is maxdeg.tab
// N (the number of samples) is the -n parameter of MAXDEGGEN
int MaxDegGen( const vector<TStr>& CommandLineArgs )
{

	srand((unsigned(time(NULL))));
                
	Env = TEnv(CommandLineArgs[MAXDEGGEN], TNotify::NullNotify);
	// output filename
	const TStr MaxDegFile = Env.GetIfArgPrefixStr("-fn:", "maxdeg.tab", "Name of output file with statistics");
	const TInt N = Env.GetIfArgPrefixInt("-n:", 1, "Number of samples to generate");

	Env = TEnv(CommandLineArgs[KRONGEN], TNotify::NullNotify);
	TStr IsDir = Env.GetIfArgPrefixStr("-isdir:", "false", "Produce directed graph (true, false)");
	const TInt NIter = Env.GetIfArgPrefixInt("-i:", 10, "Iterations of Kronecker product");
	const TInt SF = Env.GetIfArgPrefixInt("-sf:", 1, "sf - 1 is how many times the number of iterations will increase");
	const TStr ScaleMtx = Env.GetIfArgPrefixStr("-scalemtx:", "true", "Scale initiator matrix to fit for maximum degree");

	TStr InFName, OutFName;
	AddPrefix("_in", ".", MaxDegFile.CStr(), InFName);
	AddPrefix("_out", ".", MaxDegFile.CStr(), OutFName);

	ofstream OutIn(InFName.CStr());
	ofstream OutOut(OutFName.CStr());

	bool Dir = IsDir == "true" ? true : false;
	TKronMtx FitMtx;
	vector<int> MaxDegModelIn, MaxDegModelOut, ReqMaxDeg;
	// each element of vector - list of N maximum degrees for a particular size
	vector<vector<int>> MaxDegKronIn, MaxDegKronOut;
	for (int i = 0; i < SF; i++){
		MaxDegKronIn.push_back(vector<int>());
		MaxDegKronOut.push_back(vector<int>());
	}

	PyInit("PySettings.txt");
               

	// generate N samples of graph with GRAPHGEN parameter
	for (size_t i = 0; i < N; ++i){
		if ( (i + 1) % 100 == 0) 
			cout << "Sample index: " << i + 1 << endl;
		PNGraph G;
                        
		if (GetModel(CommandLineArgs[GRAPHGEN], G) == -1)
			return -1;
		MaxDegModelIn.push_back(GetMaxDeg(G,true));
		MaxDegModelOut.push_back(GetMaxDeg(G,false));
		TFile << "Model edges: " << G->GetEdges() << endl;

		// estimate initiator matrix for the first sample
		if (i == 0)
			EstimateInitMtx(CommandLineArgs, G, FitMtx);
	}

	// estimate GEV parameters from N samples
	double Scale = 0, Location = 0, Shape = 0;
	GetGevParams(CommandLineArgs[MAXDEGGEN], MaxDegModelIn, MaxDegModelOut, Scale, Location, Shape);
        
        

	// generate N samples of Kronecker graphs of various sizes
	for (size_t i = 0; i < N; ++i){
		TExeTm t;
			if ( (i + 1) % 100 == 0) 
			cout << "Sample index: " << i + 1 << endl;
		for (int j = 0; j < SF; ++j){
			if (ScaleMtx == "true"){
				// get maximum degree from GEV distribution
				int MaxDeg = GetMaxDegFromGEV(Scale, Location, Shape);
				if (MaxDeg == 0)
					return Error("GetMaxDegFromGEV()", "MaxDeg = 0");
				//cout << "Time of GetMaxDegFromGEV(): " <<  t.GetTmStr() << endl;
				if (j == 0)
					ReqMaxDeg.push_back(MaxDeg);
				// cout << "Before scaling: " << endl;
				t.Tick();
				int ErrCode = 0;
				ErrCode = ScaleFitMtx(FitMtx, NIter, static_cast<int>(pow(2.00, NIter)), MaxDeg, IsDir);
				if (ErrCode == -1)
					return -1;
				//cout << "Time of ScaleFitMtx(): " <<  t.GetTmStr() << endl;
				// cout << "After scaling: " << endl;
				// TFile << "Initiator matrix after scaling for maximum degree: " << endl;
				// FitMtx.Dump(TFile);
			}
			t.Tick();

			PNGraph Kron;
			TKronMtx::GenFastKronecker(Kron, FitMtx, NIter+j, static_cast<int>(pow(FitMtx.GetSum(), NIter+j)), Dir, rand());
			//cout << "Time of GenFastKronecker(): " <<  t.GetTmStr() << endl;
			if (i == 0) {
				int KronSize = static_cast<int>(pow(2.00, NIter+i));
				TFile << "Kron size: " << KronSize;
				TFile << " Kron edges: " << Kron->GetEdges() << endl;
			}
			t.Tick();
			MaxDegKronIn[j].push_back(GetMaxDeg(Kron, true));
			//cout << "Time of GetMaxDeg(): " <<  t.GetTmStr() << endl;
			MaxDegKronOut[j].push_back(GetMaxDeg(Kron, false));
		}
	}
    
	WriteHeader(OutIn, CommandLineArgs[GRAPHGEN], N, NIter, SF, true);
	WriteHeader(OutOut, CommandLineArgs[GRAPHGEN], N, NIter, SF, true);
	int BasicSize = static_cast<int>(NIter);
	const int Size = static_cast<int>(pow(2, BasicSize));
	WriteMaxDegSeq(OutIn, "Model", Size, MaxDegModelIn);
	WriteMaxDegSeq(OutOut, "Model", Size, MaxDegModelOut);

	for (size_t i = 0; i < SF; i++){
		const int SizeI = static_cast<int>(pow(2, BasicSize+i));
		WriteMaxDegSeq(OutIn, "Kron", SizeI, MaxDegKronIn[i]);
		WriteMaxDegSeq(OutOut, "Kron", SizeI, MaxDegKronOut[i]);
	}

	WriteMaxDegSeq(OutOut, "Kron req", Size, ReqMaxDeg);


	Py_Finalize();

        
	OutIn << "Shape = " << Shape << " Location = " << Location << " Scale = " << Scale << endl;
	OutOut << "Shape = " << Shape << " Location = " << Location << " Scale = " << Scale << endl;

	OutIn.close();
	OutOut.close();
	return 0;
}

void WriteHeader(ofstream& Out, const TStr& GraphGenPars, const int& N, const int& NIter, const int& SF, bool IsIn){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    Out << "# " << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << endl;
    if (IsIn)
        Out << "# Maximum in-degrees";
    else
        Out << "# Maximum out-degrees";
    Out << " (" << N << " samples)" << endl;
    TStr S(GraphGenPars);
    S.DelSubStr(0, 1);
    Out << "# " << GraphGenPars.CStr() << endl;
    Out << "# Basic size: " << pow(2, NIter) << endl;
    Out << "# Kronecker sizes: " << pow(2, NIter) << ", ";
    for (int i = 1; i < SF; i++){
        Out << pow(2, NIter + i);
        if (i != SF-1)
            Out << ", ";
    }
    Out << endl;
    Out << endl;
}

void WriteMaxDegSeq(ofstream& Out, const char* Name, const int& Size, const vector<int>& Deg){
    Out << Name << " (" << Size << " nodes)" << endl;
    for (auto& d: Deg)
        Out << d << " ";
    Out << endl;
    Out << endl;
}

void ReadGevParams(ifstream& Gev, double& Shape, double& Location, double& Scale){
    //cout << "Start reading GEV params..." << endl;
    string s;
    do {
        Gev >> s;
        //cout << s << endl;
    }
    while (s.find("beta") == string::npos);
    Gev >> s;
    Shape = stod(s);
    Gev >> s;
    Location = stod(s);
    Gev >> s;
    Scale = stod(s);
    cout << "Shape: " << Shape << " Location: " << Location << " Scale: " << Scale << endl;
}


void EstimateInitMtx( const vector<TStr>& CommandLineArgs, const PNGraph& G, TKronMtx& FitMtx )
{
    Env = TEnv(CommandLineArgs[KRONGEN], TNotify::NullNotify);
    TStr IsDir = Env.GetIfArgPrefixStr("-isdir:", "false", "Produce directed graph (true, false)");
    const int GNodes = G->GetNodes();
	const int NIter = GetNIter(GNodes);
	TFile << "NIter: " << NIter << endl;

    if (!GetMtx(CommandLineArgs[MTXGEN], FitMtx))
        GenNewMtx(G, CommandLineArgs[KRONFIT], FitMtx);

    TFile << "Initiator matrix before scaling for nodes and edges: " << endl;
    FitMtx.Dump(TFile);

    //FitMtx.SetForEdgesNoCut(G->GetNodes(), G->GetEdges());

    //TFile << "Initiator matrix after scaling for nodes and edges: " << endl;
    //FitMtx.Dump(TFile);

}

void GetGevParams(const TStr& MaxDegGenPars, const vector<int>& MaxDegModelIn, const vector<int>& MaxDegModelOut, double& Scale, double& Location, double& Shape){
    Env = TEnv(MaxDegGenPars, TNotify::NullNotify);
    // output filename
    const TStr RPath = Env.GetIfArgPrefixStr("-rpath:", "", "Full path to *.r file (including name)");
    const TStr ROut = Env.GetIfArgPrefixStr("-rout:", "gevfit.dat", "Name of R output file");
    // print MaxDegModelIn and MaxDegModelOut to *.dat file to use it in R
    ifstream Gev(ROut.CStr());
    ofstream OutR("MaxDegModel.dat");
    PrintIntV(MaxDegModelIn, OutR);
    PrintIntV(MaxDegModelOut, OutR);
    OutR.close();

    // execute R script to fit GEV parameters
    // full path to MaxModel.dat could be specified in *.r file
    TStr RCmd = "R CMD BATCH " + RPath;
    system(RCmd.CStr());
    cout << "R script finished..." << endl;

    ReadGevParams(Gev, Shape, Location, Scale);
    Gev.close();
}

// check x
double GEVGetMaxX( const double& Scale, const double& Location, const double& Shape ){
    if (Shape >= 0)
        return DBL_MAX;
    else
        return Location - Scale / Shape;
}

// check x
double GEVGetMinX( const double& Scale, const double& Location, const double& Shape ){
    if (Shape <= 0)
        return DBL_MIN;
    else
        return Location - Scale / Shape;
}

// returns -1 in case of invalid input
double GevCDF( const double& Scale, const double& Location, const double& Shape, double x, double mult)
{
    double y;
    if (Shape == 0)
        y = exp(-mult * exp(-(x-Location)/Scale));
    else{
        /*cout << "-1/shape: " << -1 / Shape << endl;
        cout << "(x - Location)/Scale*Shape " << (x - Location)/Scale*Shape << endl;
        cout << "pow(1 + (x - Location)/Scale*Shape, -1/Shape) " << pow(1 + (x - Location)/Scale*Shape, -1/Shape) << endl;*/
        // support
        if (Shape > 0 && x < Location - Scale / Shape)
            return -1;
        if (Shape < 0 && x > Location - Scale / Shape)
            return -1;

        y = exp(-mult * pow(1 + (x - Location)/Scale*Shape, -1/Shape));
    }
    return y;
}
