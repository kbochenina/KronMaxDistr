#include "stdafx.h"
#include "Stat.h"

int GetMaxMinDeg(const PNGraph& G, const TStr& IsDir, const TStr& IsIn, const TStr& IsMax){
    TIntPrV DegCnt;
    if (IsDir == "false"){
        PUNGraph U = TSnap::ConvertGraph<PUNGraph>(G);
        if (IsIn == "true")
            TSnap::GetInDegCnt(U, DegCnt);
        else 
            TSnap::GetOutDegCnt(U, DegCnt);
    }
    else{
        if (IsIn == "true")
            TSnap::GetInDegCnt(G, DegCnt);
        else
            TSnap::GetOutDegCnt(G, DegCnt);
    }
    // sort in descending order
    DegCnt.Sort(false);
    if (IsMax == "true") return DegCnt[0].Val1;
    else return DegCnt[DegCnt.Len()-1].Val1;
}

void CompareDeg(const int i, const int MaxDeg, int& MinMaxDeg, int& MaxMaxDeg, int& AvgMaxDeg){
    if (i == 0) { 
        MinMaxDeg = MaxDeg;
    }
    else if (MaxDeg < MinMaxDeg){
        MinMaxDeg = MaxDeg;
    }
    if (MaxDeg > MaxMaxDeg) {
        MaxMaxDeg = MaxDeg;
    }
    AvgMaxDeg += MaxDeg;
}

// get degrees from current and add it to degrees
void AddDegreeStat(const TFltPrV& current, TFltPrV& degrees){
    for (int j = 0; j < current.Len(); j++){
        const TFltPr& elem = current[j];
        const double& deg = elem.Val1.Val, &nodesCount = elem.Val2.Val;
        bool wasFound = false;
        // silly search
        for (int k = 0; k < degrees.Len(); k++){
            if (degrees[k].Val1.Val == deg){
                degrees[k].Val2.Val += nodesCount;
                wasFound = true; break;
            }
        }
        if (!wasFound){
            TFlt d(deg), n(nodesCount);
            TFltPr val(d,n);
            degrees.Add(val);
         }
    }
}

// get in or out degree count
void GetDegCnt(const PNGraph& G, TFltPrV& Seq, bool IsIn){
	TFltPrV Seq;
	if (IsIn)
		TSnap::GetInDegCnt(G, Seq);
	else 
		TSnap::GetOutDegCnt(G, Seq);
}


// get graph and add in and out degrees to cumulative vectors
void AddDegreesStat(TFltPrV& Deg, const PNGraph& G, bool IsIn){
	TFltPrV Seq;
	GetDegCnt(G, Seq, IsIn);
    AddDegreeStat(Seq, Deg);
}

void GetAvgDegreeStat (TFltPrV& deg, const TInt& NKron){
    for (int i = 0; i < deg.Len(); i++)
        deg[i].Val2.Val /= NKron;
}

// we suppose that degrees in degree sequences are sorted in ascending order
void DegSeq::CalcAvgDeg()
{
	TIntV Ind;
	for (int i = 0; i < Deg.Len(); ++i)
		Ind.Add(0);

	int DegInd = -1;

	// get cumulative values
	for (int CurrDeg = MinDeg; CurrDeg <= MaxDeg; ++CurrDeg){
		bool HasDeg = false;

		for (int i = 0; i < Deg.Len(); ++i){
			TFltPr Val = Deg[i][Ind[i]];
			// degree
			int D = static_cast<int>(Val.GetVal1());
			if ( D == CurrDeg ){
				// if this degree is for the first time
				// add it to degree sequence
				if (!HasDeg) {
					HasDeg = true;
					TFltPr Val(CurrDeg, 0);
					AvgDeg.Add(Val);
					++DegInd;
				}
				// increase count
				AvgDeg[DegInd].Val2 += Val.Val2;
				Ind[i] += 1;
			}
		}
	}

	// get average values
	for (int i = 0; i < AvgDeg.Len(); ++i){
		AvgDeg[i].Val2 /= AvgDeg.Len();
	}
}

void DegSeq::CalcSD()
{
	// if AvgDeg was not calculated yet
	if (AvgDeg.Len() == 0){
		CalcAvgDeg();
	}
	TIntV Ind;
	for (int i = 0; i < Deg.Len(); ++i)
		Ind.Add(0);

	int DegInd = -1;

	// get cumulative values
	for (int CurrDeg = MinDeg; CurrDeg <= MaxDeg; ++CurrDeg){
		bool HasDeg = false;

		for (int i = 0; i < Deg.Len(); ++i){
			TFltPr Val = Deg[i][Ind[i]];
			// degree
			int D = static_cast<int>(Val.GetVal1());
			if ( D == CurrDeg ){
				// if this degree is for the first time
				// add it to degree sequence
				if (!HasDeg) {
					HasDeg = true;
					TFltPr Val(CurrDeg, 0);
					SD.Add(Val);
					++DegInd;
				}
				// increase sum
				SD[DegInd].Val2 += pow(Val.Val2 - AvgDeg[DegInd].Val2, 2.00);
				// calculate standard deviation
				if (i == Deg.Len() - 1){
					SD[DegInd].Val2 /= Deg.Len();
					SD[DegInd].Val2 = sqrt(SD[DegInd].Val2);
				}
				Ind[i] += 1;
			}
		}
	}
}

DegSeq::DegSeq( const TVec<TFltPrV>& Val )
{
	int MinDeg = INT_MAX, MaxDeg = 0;
	for (int i = 0; i <  Val.Len(); ++i){
		TFltPrV Row;
		for (int j = 0; j <  Val[i].Len(); ++j){
			TFltPr Pr = Val[i][j];
			Row.Add(Pr);
			// for the first element in the sequence
			if (j == 0){
				if (Pr.Val1 < MinDeg)
					MinDeg = Pr.Val1;
			}
			// for the last element in the sequence
			if (j == Val[i].Len() - 1){
				if (Pr.Val1 > MaxDeg)
					MaxDeg = Pr.Val1;
			}
		}
		Deg.Add(Row);
	}
}

void DegSeq::GetAvgDegSeq( TFltPrV& AvgDegSeq )
{
	// if AvgDeg was not calculated yet
	if (AvgDeg.Len() == 0){
		CalcAvgDeg();
	}
	if (AvgDegSeq.Len() != 0)
		AvgDegSeq.Clr();
	// copy to output
	for (int i = 0; i < AvgDeg.Len(); ++i)
		AvgDegSeq.Add(AvgDeg[i]);
}

void DegSeq::GetMinErrBar( TFltPrV& MinErr )
{
	// if SD was not calculated yet
	if (SD.Len() == 0){
		CalcSD();
	}
	if (MinErr.Len() != 0)
		MinErr.Clr();
	
	if (MinErrBar.Len() == 0){
		for (int i = 0; i < AvgDeg.Len(); ++i){
			TFltPr Val(AvgDeg[i].Val1, AvgDeg[i].Val2 - SD[i].Val2);
			MinErrBar.Add(Val);
		}
	}
	// copy to output
	for (int i = 0; i < MinErrBar.Len; ++i)
		MinErr.Add(MinErrBar[i]);
}

void DegSeq::GetMaxErrBar( TFltPrV& MaxErr )
{
	// if SD was not calculated yet
	if (SD.Len() == 0){
		CalcSD();
	}
	if (MaxErr.Len() != 0)
		MaxErr.Clr();

	if (MaxErrBar.Len() == 0){
		for (int i = 0; i < AvgDeg.Len(); ++i){
			TFltPr Val(AvgDeg[i].Val1, AvgDeg[i].Val2 + SD[i].Val2);
			MaxErrBar.Add(Val);
		}
	}

	// copy to output
	for (int i = 0; i < MaxErrBar.Len; ++i)
		MaxErr.Add(MaxErrBar[i]);
}
