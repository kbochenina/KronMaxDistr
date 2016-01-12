int GetMaxMinDeg(const PNGraph& G, const TStr& IsDir, const TStr& IsIn, const TStr& IsMax);
void CompareDeg(const int i, const int MaxDeg, int& MinMaxDeg, int& MaxMaxDeg, int& AvgMaxDeg);
void AddDegreesStat(TFltPrV& Deg, const PNGraph& G, bool IsIn);
void GetAvgDegreeStat (TFltPrV& deg, const TInt& NKron);
// get in or out degree count
void GetDegCnt(const PNGraph& G, TFltPrV& Seq, bool IsIn);
