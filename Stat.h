int GetMaxMinDeg(const PNGraph& G, const TStr& IsDir, const TStr& IsIn, const TStr& IsMax);
void CompareDeg(const int i, const int MaxDeg, int& MinMaxDeg, int& MaxMaxDeg, int& AvgMaxDeg);
void AddDegreesStat(TFltPrV& Deg, const PNGraph& G, bool IsIn);
void GetAvgDegreeStat (TFltPrV& deg, const TInt& NKron);
// get in or out degree count
void GetDegCnt(const PNGraph& G, TFltPrV& Seq, bool IsIn);

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
	int MinDeg;
	int MaxDeg;
public:
	DegSeq(const TVec<TFltPrV>& Val);
	void GetAvgDegSeq(TFltPrV& AvgDegSeq);
	void GetMinErrBar(TFltPrV& MinErr);
	void GetMaxErrBar(TFltPrV& MaxErr);
};