#ifndef BEAT_DETECTOR_H
#define BEAT_DETECTOR_H

#include "spectrum.h"

#include <list>

class BeatDetector
{
public:
    BeatDetector(Spectrum* spectrum_);

    void SetSpectrum(Spectrum* spectrum_);
    void Update();

    // debugging information
    double Flux() const { return flux; }
    double Threshold() const { return threshold; }
    double ThresholdedFlux() const { return thresholded_flux; }

    // current beat value: 0 if no beat, positive value indicates size of beat
    double Beat() const { return beat; }
private:
    Spectrum* spectrum;

    // used in algorithms
    SpectrumSnapshot last_frame;
    std::list<double> flux_history;
    double last_beat;

    // current values
    double flux, threshold, thresholded_flux, beat;
};

#endif