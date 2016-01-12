#ifndef __DEGSEQ_H
#define __DEGSEQ_H

class DegSeq{
	TVec<TFltPrV> Deg;
	TFltPrV AvgDeg;
	TFltPrV SD;
	// left border of 1 SD
	TFltPrV MinErrBar;
	// right border of 1 SD
	TFltPrV MaxErrBar;
	void CalcAvgDeg();
	void CalcSD();
	int CheckSeqLen();
	int MinDeg;
	int MaxDeg;
public:
	DegSeq(const TVec<TFltPrV>& Val);
	void GetAvgDegSeq(TFltPrV& AvgDegSeq);
	void GetMinErrBar(TFltPrV& MinErr);
	void GetMaxErrBar(TFltPrV& MaxErr);
	bool CheckDeg();
};

#endif