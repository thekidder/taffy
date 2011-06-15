#ifndef ENERGY_ANALYZER_H
#define ENERGY_ANALYZER_H

#include "analyzer.h"
#include "spectrum.h"

class EnergyAnalyzer : public Analyzer
{
public:
    EnergyAnalyzer(Spectrum* spectrum);

    void AddSpectrum(Spectrum* spectrum) { spectrums.push_back(spectrum); }
    double Energy(size_t index) const { return Value(index); }

    virtual void Update();

    virtual size_t NumValues() const { return spectrums.size(); }
    virtual double Value(size_t index) const;

    virtual Double_range_t Range() const { return std::make_pair(0.0, 60.0); }
private:
    std::vector<Spectrum*> spectrums;

    std::vector<double> energies;
};

#endif