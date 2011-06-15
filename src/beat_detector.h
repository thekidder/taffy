#ifndef BEAT_DETECTOR_H
#define BEAT_DETECTOR_H

#include "analyzer.h"
#include "bounded_queue.h"
#include "spectrum.h"

#include <list>

class BeatDetector : public Analyzer
{
public:
    BeatDetector(Spectrum* spectrum_);

    void SetSpectrum(Spectrum* spectrum_);
    double Flux() const { return flux; }
    double Threshold() const { return threshold; }
    double ThresholdedFlux() const { return thresholded_flux; }
    // current beat value: 0 if no beat, positive value indicates size of beat
    double Beat() const { return beat; }

    virtual void Update();

    virtual size_t NumValues() const { return 4; }
    virtual double Value(size_t index) const;

    virtual Double_range_t Range() const { return std::make_pair(0.0, 30.0); }
private:
    Spectrum* spectrum;

    // used in algorithms
    SpectrumSnapshot last_frame;
    Bounded_queue<double> flux_history, threshold_history;
    double last_beat;

    // current values
    double flux, threshold, thresholded_flux, beat;
};

#endif