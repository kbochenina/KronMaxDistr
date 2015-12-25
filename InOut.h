
int Error(const TStr& FuncName, const TStr& ErrorMsg);
int ReadParameters(TStr settingsFN, vector<TStr>& out);
void PlotDegrees(const vector <TStr>& Parameters, const TFltPrV& In, const TFltPrV& Out, const TStr& Type);
void PlotDeg( const PNGraph& G, const TStr& Type, const TStr& Plt );
int PlotDegKron( const CmdArgs& Args, const TKronMtx& FitMtx, const TStr& Type);

void GetParameters(const vector<TStr>& CommandLineArgs, vector<TStr>& Parameters);
void PrintMtx(const TKronMtx& FitMtxM, ofstream& TFile);
// read graph from file
void ReadPNGraphFromFile(const TStr args, PNGraph& G);
void ReadPNGraphFromFile(const TStr FName, PNGraph& G);
ofstream OpenFile(const TStr& fileName, bool& IsError);
void ReadMtx(const TStr& Mtx, const TInt& MtxSize, TKronMtx& FitMtx);
// print the vector of TStr
void PrintTStrV(const vector<TStr>& CommandLineArgs);
// print the vector of TFltPr
void PrintTFltPrV(const TFltPrV& V);
// Res is a string where Pref is added before Sep in string S
void AddPrefix(const char* Pref, const char * Sep, const char* S, TStr& Res);
// print vector of int to file
void PrintIntV(const vector<int>& V, ofstream& Out);
// check file for existence
bool CheckFile(const TStr FName);

