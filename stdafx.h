// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <string>

const int NFUNC = 7;
//const std::string FUNCNAMES[] = {"KRONTEST", "GRAPHGEN", "MTXGEN", "KRONFIT", "KRONGEN", "MAXDEGGEN", "TESTGRAPH"};
//const enum ARGS { KRONTEST = 0, GRAPHGEN = 1, MTXGEN = 2, KRONFIT = 3, KRONGEN = 4, MAXDEGGEN = 5, TESTGRAPH = 6, PTYPE = 7, PLT = 8, NAME = 9};

const std::string FUNCNAMES[] = {"KRONTEST", "GRAPHGEN", "MTXGEN", "KRONFIT", "KRONGEN", "MAXDEGGEN", "TESTGRAPH"};
const enum ARGS { KRONTEST = 0, GRAPHGEN = 1, MTXGEN = 2, KRONFIT = 3, KRONGEN = 4, MAXDEGGEN = 5, PTYPE = 7, PLT = 8, NAME = 9, TESTGRAPH = 10};

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <vector>
#include "Snap.h"
#include "Python.h"
#include "kronecker.h"
#include <fstream>
#include <stdlib.h>
#include <map>
#include <tuple>
#include <iostream>

using namespace std;

extern ofstream TFile;


// TODO: reference additional headers your program requires here
