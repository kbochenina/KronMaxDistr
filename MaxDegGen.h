
#include "InOut.h"
#include "GenPy.h"
#include "KronGen.h"
#include <iomanip>
#include <ctime>

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
void MaxDegGen(const vector<TStr>& CommandLineArgs);
int GetMaxDeg(const PNGraph& G, bool IsIn);
void WriteHeader(ofstream& Out, const TStr& GraphGenPars, const int& N, const int& NIter, const int& SF, bool IsIn);
void WriteMaxDegSeq(ofstream& Out, const char* Name, const int& Size, const vector<int>& Deg);