#include "energy_analyzer.h"

EnergyAnalyzer::EnergyAnalyzer(Spectrum* spectrum)
{
    if(spectrum)
        AddSpectrum(spectrum);
}

void EnergyAnalyzer::Update()
{
    energies.clear();

    for(std::vector<Spectrum*>::iterator it = spectrums.begin(); it != spectrums.end(); ++it)
    {
        double energy = 0.0;
        for(size_t i = 0; i < (*it)->NumBuckets(); ++i)
        {
            // -60dB is our bottom sensitivity
            double bucket_energy = 60.0 + (*it)->Value(i, STEREO_MIX);
            bucket_energy = std::max(0.0, bucket_energy);
            energy += bucket_energy;
        }
        energy /= (*it)->NumBuckets();

        energies.push_back(energy);
    }
}

double EnergyAnalyzer::Value(size_t index) const
{
    if(index >= energies.size())
        return 0.0;
    return energies[index];
}