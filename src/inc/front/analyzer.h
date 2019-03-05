#ifndef PL01_FRONT_ANALYZER_H_
#define PL01_FRONT_ANALYZER_H_

#include <string>

#include <define/symbol.h>

class Analyzer {
public:
    Analyzer() {}

    SymbolType AnalyzeConst(const std::string &id, unsigned int line_num);

private:
    EnvPtr env_;
};

#endif // PL01_FRONT_ANALYZER_H_
