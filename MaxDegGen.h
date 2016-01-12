
#include "InOut.h"
#include "GenPy.h"
#include "Scale.h"
#include <iomanip>
#include <ctime>
#include "KronGen.h"

void GetRndDegSeq(const PNGraph& G, int Size, TRnd& Rnd, TIntV& DegV);
void GetRndNIdV(const PNGraph& G, int Size, TRnd& Rnd, TIntV& NIdV);
void GetSubGraph(int Size, int GenType, const PNGraph& G, vector<PNGraph>& SubGraphs, int& MaxDeg);
void TestRealGraph(const vector<TStr>& CommandLineArgs);
int MaxDegGen(const vector<TStr>& CommandLineArgs);
int GetMaxDeg(const PNGraph& G, bool IsIn);
int GetMaxDeg(const PNGraph& G);
void WriteHeader(ofstream& Out, const TStr& GraphGenPars, const int& N, const int& NIter, const int& SF, bool IsIn);
void WriteMaxDegSeq(ofstream& Out, const char* Name, const int& Size, const vector<int>& Deg);
void ReadGevParams(ifstream& Gev, double& Shape, double& Location, double& Scale);
void EstimateInitMtx(const vector<TStr>& CommandLineArgs, const PNGraph& G, TKronMtx& FitMtx, double& LL = DEFLL, bool SavePerm = false);
void GetGevParams(const TStr& MaxDegGenPars, const vector<int>& MaxDegModelIn, const vector<int>& MaxDegModelOut, double& Scale, double& Location, double& Shape);
double GevCDF(const double& Scale, const double& Location, const double& Shape, double x, double mult = 1);
double GEVGetMaxX(const double& Scale, const double& Location, const double& Shape);
double GEVGetMinX(const double& Scale, const double& Location, const double& Shape);