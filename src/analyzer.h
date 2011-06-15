#ifndef ANALYZER_H
#define ANALYZER_H

#include "utility.h"

// interface for some sort of analyzer that can be graphed
class Analyzer
{
public:
    virtual void Update() = 0;

    virtual size_t NumValues() const = 0;
    virtual double Value(size_t index) const = 0;

    virtual Double_range_t Range() const = 0;
};

#endif