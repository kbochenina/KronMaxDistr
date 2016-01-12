#include "stdafx.h"
#include "InOut.h"
#include "DegSeq.h"

#define DEBUG_DEG_SEQ 1
#define DEBUG_LEV2_DEG_SEQ 0

// we suppose that degrees in degree sequences are sorted in ascending order
void DegSeq::CalcAvgDeg()
{
	TIntV Ind;
	for (int i = 0; i < Deg.Len(); ++i)
		Ind.Add(0);

	int DegInd = -1;

	// get cumulative values
	for (int CurrDeg = MinDeg; CurrDeg <= MaxDeg; ++CurrDeg){

		if (DEBUG_LEV2_DEG_SEQ) 
			printf("CurrDeg: %d\n", CurrDeg);

		bool HasDeg = false;

		for (int i = 0; i < Deg.Len(); ++i){
			if (Ind[i] > Deg[i].Len() - 1)
				continue;
			TFltPr Val = Deg[i][Ind[i]];
			// degree
			int D = static_cast<int>(Val.GetVal1());

			if (DEBUG_LEV2_DEG_SEQ){
				printf("Deg: %f Val: %f\n", Deg[i][Ind[i]].Val1, Deg[i][Ind[i]].Val2);
				printf("i: %d Degree: %d Ind[%d]: %d Val: %f\n", i, D, i, Ind[i], Val.Val2);
			}

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

				if (DEBUG_LEV2_DEG_SEQ)
					printf("AvgDeg[%d] = %f\n", DegInd, AvgDeg[DegInd].Val2);
			}
		}
	}

	// get average values
	for (int i = 0; i < AvgDeg.Len(); ++i){
		AvgDeg[i].Val2 /= Deg.Len();
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
		int CurrDegCount = 0;
		bool HasDeg = false;

		for (int i = 0; i < Deg.Len(); ++i){
			if (Ind[i] > Deg[i].Len() - 1)
				continue;
			++CurrDegCount;

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
				Ind[i] += 1;
			}
		}
		int DegAbsent = Deg.Len() - CurrDegCount;
		for (int i = 0; i < DegAbsent; ++i)
			SD[DegInd].Val2 += pow(- AvgDeg[DegInd].Val2, 2.00);
		// calculate standard deviation
		SD[DegInd].Val2 /= Deg.Len();
		SD[DegInd].Val2 = sqrt(SD[DegInd].Val2);
	}
}

DegSeq::DegSeq( const TVec<TFltPrV>& Val )
{
	MinDeg = INT_MAX;
	MaxDeg = 0;
	for (int i = 0; i <  Val.Len(); ++i){
		TFltPrV Row;
		// PrintTFltPrV(Val[i]);
		for (int j = 0; j <  Val[i].Len(); ++j){
			TFltPr Pr = Val[i][j];
			//printf("Pr: %f %f\n", Pr.Val1, Pr.Val2);
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
		// printf("Row was added\n");
	}

	if (DEBUG_DEG_SEQ) {
		printf("MinDeg: %d MaxDeg: %d\n", MinDeg, MaxDeg);
	
		for (int i = 0; i < Deg.Len(); ++i){
			PrintTFltPrV(Deg[i]);
		}
	}
}

void DegSeq::GetAvgDegSeq( TFltPrV& AvgDegSeq )
{
	// if AvgDeg was not calculated yet
	if (AvgDeg.Len() == 0){
		CalcAvgDeg();
	}

	if (DEBUG_DEG_SEQ) 
		PrintTFltPrV(AvgDeg);

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

	if (DEBUG_DEG_SEQ){
		printf("Standard deviation:\n");
		PrintTFltPrV(SD);
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
	for (int i = 0; i < MinErrBar.Len(); ++i)
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
	for (int i = 0; i < MaxErrBar.Len(); ++i)
		MaxErr.Add(MaxErrBar[i]);
}

int DegSeq::CheckSeqLen()
{
	int DegLen = AvgDeg.Len(), MinLen = MinErrBar.Len(), MaxLen = MaxErrBar.Len();
	if (DegLen != MinLen ||
		DegLen != MaxLen ||
		MaxLen != MinLen)
		return Error("DegSeq::CheckSeqLen()", "Sequence length is not consistent");
	return 0;
}

bool DegSeq::CheckDeg()
{
	if (CheckSeqLen() == -1) 
		return false;
	for (int i = 0; i < AvgDeg.Len(); ++i){
		int DegVal = AvgDeg[i].Val1, MinVal = MinErrBar[i].Val1, MaxVal = MaxErrBar[i].Val1;
		if (DegVal != MinVal ||
			DegVal != MaxVal ||
			MaxVal != MinVal){
				Error("DegSeq::CheckDeg()", "Degrees in the sequence are not consistent");
				return false;
		}
	}
	return true;
}

