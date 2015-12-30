#include <vector>
#include "kronecker.h"

class CmdArgs{
	std::vector<TStr> CommandLineArgs;
public:
	CmdArgs(const std::vector<TStr>& CommandLineArgs);
	bool IsDir() const;
	TStr IsDirStr() const;
	int GetLine(const int Index, TStr& Line) const;
	void GetArgsArray(std::vector<TStr>& Args) const;
	int GetMinSize() const;
	int GetSF() const;
	TStr GetPlt() const;
	int GetGDRuns() const;
};