#include "beat_detector.h"

#include <algorithm>
#include <limits>
#include <numeric>

const size_t k_history_size = 100;
const size_t k_num_reference_beats = 10;
const double k_default_threshold_value = 1.5;

BeatDetector::BeatDetector(Spectrum* spectrum_)
    : spectrum(spectrum_), last_frame(0),
      flux_history(k_history_size), threshold_history(k_history_size)
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
            diff = diff > 0.0 ? diff : 0.0;
            flux += diff;
        }
        flux /= spectrum->NumBuckets();

        
        double flux_moving_average = std::accumulate(flux_history.begin(), flux_history.end(), 0.0);
        if(flux_history.size())
            flux_moving_average /= flux_history.size();

        double threshold_mult;
        if(flux_history.size() == k_history_size)
        {
            // find averages of beats/non-beats
            double beat_avg = 0.0, normal_avg = 0.0;
            size_t num_beats = 0, num_normal = 0;
            Bounded_queue<double>::const_iterator i, j;

            for(i = flux_history.begin(), j = threshold_history.begin(); 
                i != flux_history.end() && j != threshold_history.end();
                ++i, ++j)
            {
                if(*i > *j)
                {
                    beat_avg += *i;
                    ++num_beats;
                }
                else
                {
                    normal_avg += *i;
                    ++num_normal;
                }
            }

            if(num_beats && num_normal)
            {
                beat_avg /= num_beats;
                normal_avg /= num_normal;

                double t = (beat_avg - normal_avg) / 2.0 + normal_avg;
                threshold_mult = t / flux_moving_average;
            }
            else
            {
                threshold_mult = k_default_threshold_value;
            }
        }
        else
        {
            threshold_mult = k_default_threshold_value;
        }


        threshold = threshold_mult * flux_moving_average;

        thresholded_flux = flux - threshold < 0 ? 0.0 : flux - threshold;
        beat = thresholded_flux > 0.0 ? flux : 0.0;
        if(beat < last_beat)
            beat = 0.0;

        flux_history.push(flux);
        threshold_history.push(threshold);

        last_beat = beat;
    }

    last_frame = spectrum->Snapshot();
}