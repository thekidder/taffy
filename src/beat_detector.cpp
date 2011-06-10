#include "beat_detector.h"

const size_t k_history_size = 11;
const double k_threshold_value = 1.5;

BeatDetector::BeatDetector(Spectrum* spectrum_)
    : spectrum(spectrum_), last_frame(0)
{
}

void BeatDetector::SetSpectrum(Spectrum* spectrum_)
{
    spectrum = spectrum_;
}

void BeatDetector::Update()
{
    // only compute flux when this frame and the last frame have the same # of buckets
    // i.e. don't do this computation when we change spectrums, could result in a crash
    if(last_frame.NumBuckets() == spectrum->NumBuckets())
    {
        flux = 0.0f;
        for(size_t i = 0; i < spectrum->NumBuckets(); ++i)
        {
            double diff = spectrum->Value(i, STEREO_MIX) - last_frame.Value(i, STEREO_MIX);
            diff = diff > 0.0f ? diff : 0.0f;
            flux += diff;
        }
        flux /= spectrum->NumBuckets();

        if(flux_history.size() == k_history_size)
            flux_history.pop_front();
        flux_history.push_back(flux);

        double flux_moving_average = 0;
        for(std::list<double>::reverse_iterator it = flux_history.rbegin(); it != flux_history.rend(); ++it)
        {
            flux_moving_average += *it;
        }
        if(flux_history.size())
            flux_moving_average /= flux_history.size();

        threshold = k_threshold_value * flux_moving_average;
        thresholded_flux = flux - threshold < 0 ? 0.0f : flux - threshold;
        beat = thresholded_flux;
        if(beat > last_beat)
            beat = thresholded_flux;

        last_beat = beat;
    }

    last_frame = spectrum->Snapshot();
}
