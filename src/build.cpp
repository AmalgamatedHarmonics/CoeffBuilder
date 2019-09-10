#include <iostream>
#include <string>
#include <array>
#include <vector>

#include "fidlib.h"

const int NUM_FREQS = 21;
const int NUM_SCALES = 11;
const double PI  = 3.141592653589793238463;

struct scale {

    std::string classname;
    std::string name;
    std::string description;
    std::array<std::string,11> scalename;
    std::array<double,231> frequency;
    std::array<std::string,231> notename;  

};

struct generator {
    // This must generate a complete scale object
    virtual scale generateScale() = 0;
};

struct video_generator : generator {

	double start_freq = 59.94;

    scale generateScale() {
        scale m;
        m.classname = "video_notused";
        m.name = "Video";
        m.description = "Scales derived from the NTSC frame rate of 59.94Hz, each note is the next multiple of base frequency. This is not used in SMR";
        m.scalename = {
            "Video H 1; 59.94hz-",
            "Video H 2; 119.88Hz-",
            "Video H 3; 179.82Hz-",
            "Video H 4; 239.76Hz-",
            "Video H 5; 299.70Hz-",
            "Video H 6; 359.64Hz-",
            "Video H 7; 419.58Hz-",
            "Video H 8; 479.52Hz-",
            "Video H 9; 539.46Hz-",
            "Video H 10; 599.40Hz-",
            "Video V 1; 15729.00Hz-"
        };

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            if (scaleIdx < 10) {
                for (int noteIdx = 0; noteIdx < NUM_FREQS; noteIdx++) {
                    m.frequency[scaleIdx * NUM_FREQS + noteIdx] = start_freq * (noteIdx + 1) * (scaleIdx + 1);
                }
            } else {
                for (int noteIdx = 0; noteIdx < 11; noteIdx++) {
                    m.frequency[scaleIdx * NUM_FREQS + noteIdx] = 15729 + noteIdx;
                }
                for (int noteIdx = 11; noteIdx < NUM_FREQS; noteIdx++) {
                    m.frequency[scaleIdx * NUM_FREQS + noteIdx] = 31463 + noteIdx;
                }
            }
        }

        return m;
    };

};

struct filter {

    double frequencyCut = 20000.0;
    bool frequencyCutEnabled = false;
    double lastValid = -1.0;

    // Must return coefficients corresponding to the filter-calibrated frequency.
    // If frequency cut enabled:
    //   If the requested frequency exceeds the hard cut and valid coefficients have been previously generated, return those coefficients
    //   If the requested frequency exceeds the hard cut and no valid coefficients have been previously generated, return the coefficients corresponding the to hard cut.
    std::vector<double> generateCoeffs(double frequency) {
        if (frequencyCutEnabled) {
            if (frequency > frequencyCut) {
                if (lastValid != -1.0) {
                    return calculate(lastValid);
                } else {
                    return calculate(frequencyCut);
                }
            } else {
                lastValid = frequency;
                return calculate(frequency);
            }
        } else {
            return calculate(frequency);
        }
    }

    virtual std::vector<double> calculate(double frequency) = 0;

};

struct maxq_filter : filter {

    int sampleRate = 96000;

    std::vector<double> calculate(double frequency) override {
        std::vector<double> v;
        v.push_back(2.0 * PI * calibrateFrequency(frequency) / (double)sampleRate);
        return v;
    }

    double calibrateFrequency(double frequency) {
        return frequency;
    }

};

struct bpre_filter : filter {

    int Qval = 800;
  	double gain_q = 40;
    int sampleRate = 96000;

    std::vector<double> calculate(double frequency) override {

	    char str[80];
      	char *desc;

        double f = calibrateFrequency(frequency);

        sprintf(str, "BpRe/%d/%g", Qval, f);

        FidFilter *filt = fid_design(str, sampleRate, 0, 0, 0, &desc);
        filt = FFNEXT(filt);

        int len = filt->len;

        double gain_adj = fid_response(filt, f / (double)sampleRate);
        gain_adj = gain_q / gain_adj;

        std::vector<double> v;
        v.push_back(gain_adj);
        v.push_back(filt->val[2]);
        v.push_back(filt->val[1]);

        std::cout << frequency << " " << gain_adj << " " << filt->val[2] << " " << filt->val[1] << std::endl;

        return v;

    }

    double calibrateFrequency(double f) {
        return f;
    }

};

int main() {

    video_generator video_g = {};
    bpre_filter filter = {};

    scale video_s = video_g.generateScale();

    for (auto f: video_s.frequency) {
        std::vector<double> c = filter.generateCoeffs(f);
    }

}