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
void MaxDegGen( const vector<TStr>& CommandLineArgs )
{
    Try
        srand((unsigned(time(NULL))));
        Env = TEnv(CommandLineArgs[KRONTEST], TNotify::NullNotify);
        // file name with statistics
        const TStr StatFile = Env.GetIfArgPrefixStr("-ot:", "stat.tab", "Name of output file with statistics");
        // TFile is global variable
        TFile = OpenFile(StatFile.CStr());
        
        Env = TEnv(CommandLineArgs[MAXDEGGEN], TNotify::NullNotify);
        // output filename
        const TStr MaxDegFile = Env.GetIfArgPrefixStr("-fn:", "maxdeg.tab", "Name of output file with statistics");
        const TInt N = Env.GetIfArgPrefixInt("-n:", 1, "Number of samples to generate");
        const TStr RPath = Env.GetIfArgPrefixStr("-rpath:", "", "Full path to *.r file (including name)");
        const TStr ROut = Env.GetIfArgPrefixStr("-rout:", "gevfit.dat", "Name of R output file");

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
        ifstream Gev(ROut.CStr());

        bool Dir = IsDir == "true" ? true : false;
        TKronMtx FitMtx;
        vector<int> MaxDegModelIn, MaxDegModelOut;
        // each element of vector - list of N maximum degrees for a particular size
        vector<vector<int>> MaxDegKronIn, MaxDegKronOut;
        for (int i = 0; i < SF; i++){
            MaxDegKronIn.push_back(vector<int>());
            MaxDegKronOut.push_back(vector<int>());
        }

        PyInit("PySettings.txt");
               

        // generate N samples of graph with GRAPHGEN parameter
        for (size_t i = 0; i < N; ++i){
            cout << "Number of the sample: " << i + 1 << endl;
            PNGraph G;
                        
            GetModel(CommandLineArgs[GRAPHGEN], G);
            MaxDegModelIn.push_back(GetMaxDeg(G,true));
            MaxDegModelOut.push_back(GetMaxDeg(G,false));
            TFile << "Model edges: " << G->GetEdges() << endl;


            // estimate initiator matrix for the first sample
            if (i == 0){
                if (!GetMtx(CommandLineArgs[MTXGEN], FitMtx))
                    GenNewMtx(G, CommandLineArgs[KRONFIT], FitMtx);

                TFile << "Initiator matrix before scaling for nodes and edges: " << endl;
                FitMtx.Dump(TFile);

                FitMtx.SetForEdgesNoCut(G->GetNodes(), G->GetEdges());

                TFile << "Initiator matrix after scaling for nodes and edges: " << endl;
                FitMtx.Dump(TFile);

                if (ScaleMtx == "true"){
                    ScaleFitMtx(FitMtx, NIter, G->GetNodes(), GetMaxDeg(G,false), IsDir);
                    TFile << "Initiator matrix after scaling for maximum degree: " << endl;
                    FitMtx.Dump(TFile);
                }
            }

            for (int i = 0; i < SF; ++i){
                // matrix here is without any scaling
                PNGraph Kron = TKronMtx::GenFastKronecker(FitMtx, NIter+i, Dir, rand());
                int KronSize = pow(2.00, NIter+i);
                TFile << "Kron size: " << KronSize;
                TFile << " Kron edges: " << Kron->GetEdges() << endl;

                MaxDegKronIn[i].push_back(GetMaxDeg(Kron, true));
                MaxDegKronOut[i].push_back(GetMaxDeg(Kron, false));
            }
        }
    
        WriteHeader(OutIn, CommandLineArgs[GRAPHGEN], N, NIter, SF, true);
        WriteHeader(OutOut, CommandLineArgs[GRAPHGEN], N, NIter, SF, true);
        int BasicSize = static_cast<float>(NIter);
        WriteMaxDegSeq(OutIn, "Model", pow(2, BasicSize), MaxDegModelIn);
        WriteMaxDegSeq(OutOut, "Model", pow(2, BasicSize), MaxDegModelOut);

        for (size_t i = 0; i < SF; i++){
            WriteMaxDegSeq(OutIn, "Kron", pow(2, BasicSize+i), MaxDegKronIn[i]);
            WriteMaxDegSeq(OutOut, "Kron", pow(2, BasicSize+i), MaxDegKronOut[i]);
        }


        Py_Finalize();

        // print MaxDegModelIn and MaxDegModelOut to *.dat file to use it in R
        ofstream OutR("MaxDegModel.dat");
        PrintIntV(MaxDegModelIn, OutR);
        PrintIntV(MaxDegModelOut, OutR);
        OutR.close();
        
        // execute R script to fit GEV parameters
        // full path to MaxModel.dat could be specified in *.r file
        TStr RCmd = "R CMD BATCH " + RPath;
        system(RCmd.CStr());
        cout << "R script finished..." << endl;

        double Scale = 0, Location = 0, Shape = 0;

        ReadGevParams(Gev, Shape, Location, Scale);
        OutIn << "Shape = " << Shape << " Location = " << Location << " Scale = " << Scale << endl;
        OutOut << "Shape = " << Shape << " Location = " << Location << " Scale = " << Scale << endl;

        OutIn.close();
        OutOut.close();
        Gev.close();
    Catch
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
    string s;
    do {
        Gev >> s;
    }
    while (s.find("beta") == string::npos);
    Gev >> s;
    Shape = stod(s);
    Gev >> s;
    Location = stod(s);
    Gev >> s;
    Scale = stod(s);
}
