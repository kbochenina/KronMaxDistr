#include "stdafx.h"
#include "InOut.h"
#include "KronGen.h"
#include "Stat.h"
#include "DegSeq.h"

int Error(const TStr& FuncName, const TStr& ErrorMsg){
    printf("Error in %s: %s\n", FuncName.CStr(), ErrorMsg.CStr());
    system("pause");
    return -1;
}


int ReadParameters(TStr settingsFN, vector<TStr>& out){
    ifstream f;
    f.open(settingsFN.CStr());
    if (!f.is_open())
        return Error("ReadParameters", "File with settings (parameters.txt by default) is not found");
    string insteadOfName = "1 ";
    for (int i = 0; i < NFUNC; i++){
        string s;
        getline(f,s);
        if ( s != FUNCNAMES[i])
            return Error("ReadParameters", "Wrong syntax in file of settings");
			
        bool isComment = true;
        while (isComment){
            getline(f,s);
            if (s.find_first_of("//")!=0)
                isComment = false;
        }

        if (s == "default")
            s = insteadOfName;
        else s = insteadOfName + s;
        TStr ts(s.c_str());
        out.push_back(ts);
    }
    f.close();
	return 0;
}

void GetNEFromAccDistr(const TFltPrV& deg, int& nodes, int& edges){
    double nodesD = deg[0].Val2.Val, edgesD = 0;
    for (int i = 0; i < deg.Len(); i++){
        if (i == deg.Len()-1)
            edgesD += deg[i].Val1.Val * deg[i].Val2.Val;
        else edgesD += deg[i].Val1.Val * (deg[i].Val2.Val - deg[i+1].Val2.Val);
    }
    nodes = static_cast<int>(nodesD);
    edges = static_cast<int>(edgesD);
    //	edges /= 2; as Deg = inDeg + outDeg
}

void SaveDegree(const TFltPrV& deg, const TStr& n, bool isIn, bool isCum){
    TFltPrV d(deg);
    d.Sort();
    int nodes, edges;
    GetNEFromAccDistr(d, nodes, edges);
    if (isCum){
        d = TGUtil::GetCCdf(d);
    }
    TSnap::PlotDegDistr(d, nodes, edges, n, n, isCum, false, isIn);
}

// plot all points without binning
void PlotPoints(const TFltPrV& in, const TFltPrV& out, const TStr& name, const TStr& Plt){
    if (Plt == "noncum" || Plt == "all"){
        SaveDegree(in, name, true, false);
        SaveDegree(out, name, false, false);
    }
    if (Plt == "cum" || Plt == "all"){
        SaveDegree(in, name, true, true);
        SaveDegree(out, name, false, true);
    }
}

void PlotDegrees(const vector <TStr>& Parameters, const TFltPrV& In, const TFltPrV& Out, const TStr& Type){
    const TStr& Name = Parameters[NAME];
    const TStr& Plt = Parameters[PLT];
    PlotPoints(In, Out, Type + Name, Plt);
   
}



void PlotDeg( const PNGraph& G, const TStr& Type, const TStr& Plt )
{
	TFltPrV MDegIn, MDegOut;
	TSnap::GetInDegCnt(G, MDegIn);
	TSnap::GetOutDegCnt(G, MDegOut);
	PlotPoints(MDegIn, MDegOut, Type, Plt);
}

int PlotDegKron( const CmdArgs& Args, const TKronMtx& FitMtx, const TStr& Type)
{
	vector<TStr> CommandLineArgs;
	Args.GetArgsArray(CommandLineArgs);

	TStr KronGenLine;
	if (Args.GetLine(KRONGEN, KronGenLine) == -1) 
		return -1;

	TFltPrV KronDegAvgIn, KronDegAvgOut;
	GenKron(Args, FitMtx, KronDegAvgIn, KronDegAvgOut);
	PlotPoints(KronDegAvgIn, KronDegAvgOut, Type, Args.GetPlt());
	return 0;
}

void PrintMtx(const TKronMtx& FitMtxM, ofstream& TFile){
    TFile << "Initiator matrix: ";
    int Dim = FitMtxM.GetDim();
    for (int i = 0; i < Dim; ++i)
        for (int j = 0; j < Dim; ++j){
            TFile << FitMtxM.At(i,j);
            if (!(i == Dim-1 && j == Dim-1))
                TFile << ";";
        }
    TFile << endl;
}

void GetParameters(const vector<TStr>& CommandLineArgs, vector<TStr>& Parameters){
    Env = TEnv(CommandLineArgs[KRONTEST], TNotify::NullNotify);
    // to plot
    const TStr Plt = Env.GetIfArgPrefixStr("-plttype:", "model+kron", "Plotting of big model and/or its Kronecker product (model, kron, model+kron)");
    // type of plots
    const TStr PltType = Env.GetIfArgPrefixStr("-plt:", "all", "Type of plots (cum, noncum, all)");
    
    for (size_t i = 0; i < CommandLineArgs.size(); i++)
        Parameters.push_back(CommandLineArgs[i]);
    Parameters.push_back(Plt); Parameters.push_back(PltType); 
}




ofstream OpenFile(const TStr& fileName, bool& IsError)
{
	IsError = false;
    ofstream f(fileName.CStr());
    if (f.is_open())
        return f;
	Error("OpenFile()", "File " + fileName  + " was not open");
	IsError = true;
	return f;
}

void GetMtxFromSepLine(const TStr& line, const TStr& separator, TFltV& matrix){
    TStrV strVals;
    line.SplitOnAllAnyCh(separator, strVals);
    for (int i = 0; i < strVals.Len(); i++) 
        matrix.Add(strVals[i].GetFlt());
}

void ReadMtx(const TStr& Mtx, const TInt& MtxSize, TKronMtx& FitMtx){
    TFltV matrix;
    GetMtxFromSepLine(Mtx, ";", matrix);
    FitMtx.GenMtx(matrix.Len() / MtxSize);
    FitMtx.SetMtx(matrix);
}

// print the vector of TStr
void PrintTStrV(const vector<TStr>& CommandLineArgs){
    for (const TStr& s : CommandLineArgs)
        cout << s.CStr() << endl;
}

// print the vector of TFltPr
void PrintTFltPrV(const TFltPrV& V){
    for (int i = 0; i < V.Len(); ++i)
        cout << "(" << V[i].GetVal1() << ", " << V[i].GetVal2() << ")" << " ";
    cout << endl;
}

// print vector of int to file
void PrintIntV(const vector<int>& V, ofstream& Out){
    for (size_t i = 0; i < V.size(); ++i)
        Out << V[i] << " ";
    Out << endl;
}

bool CheckFile( const TStr FName )
{
	ifstream f(FName.CStr(), ios::in);
	if (!f)
		return false;
	return true;
}

bool SaveDegErrBars( DegSeq& Seq, const TStr& NameTStr, bool IsIn )
{
	TFltPrV AvgDeg, MinErr, MaxErr;
	Seq.GetAvgDegSeq(AvgDeg);
	Seq.GetMinErrBar(MinErr);
	Seq.GetMaxErrBar(MaxErr);
	//PrintTFltPrV(AvgDeg);
	//PrintTFltPrV(MinErr);
	//PrintTFltPrV(MaxErr);
	if (Seq.CheckDeg() == false)
		return false;

	ofstream F(NameTStr.CStr());
	if (!F)
		return false;
	F << "#" << endl;
	F << "#" << NameTStr.CStr() << endl; 
	F << "#" << endl;
	F << "#" << std::setw(10) << left << (IsIn ? "In" : "Out") << "-degree" << std::setw(10) << left << "AvgCount" << std::setw(10) << left << 
		"MinErrBar" << std::setw(10) << left << "MaxErrBar" << endl;
	for (int i = 0; i < AvgDeg.Len(); ++i){
		F << std::setw(10) << left << AvgDeg[i].Val1 << std::setw(10) << left  << AvgDeg[i].Val2 << std::setw(10) << left << MinErr[i].Val2 << std::setw(10) << left << MaxErr[i].Val2 << endl;
	}
	return true;
}

// Res is a string where Pref is added before Sep in string S
void AddPrefix(const char* Pref, const char * Sep, const char* S, TStr& Res){
    const int MaxSize = 100;
    if (strlen(S) > MaxSize || strlen(Pref) > MaxSize){
        cout << "AddPrefix() warning. String is too large. Prefix will not be added";
        Res = S;
    }

    const char* pch;
    pch = strstr (S, Sep);
    if (pch == nullptr)
        Res = S;
    char Begin[MaxSize], End[MaxSize];
    strncpy(Begin, S, pch - S);
    strncpy(End, pch, strlen(S) - strlen(Begin));
    Res = Begin;
    Res += Pref;
    Res += End;
}