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

struct bp_generator : generator {

	double start_freq=32.7031956626; //C1

   	double BP_intervals[14] = {
        1, 
        27.0/25, 
        25.0/21.0, 
        9.0/7.0, 
        7.0/5.0, 
        75.0/49.0, 
        5.0/3.0, 
        9.0/5.0, 
        49.0/25.0, 
        15.0/7.0, 
        7.0/3.0, 
        63.0/25.0, 
        25.0/9.0, 
        3
    };

   	std::string BP_intervals_str[14] = {
        "C", 
        "Db", 
        "D", 
        "E", 
        "F", 
        "Gb", 
        "G", 
        "H", 
        "Jb", 
        "J", 
        "A", 
        "Bb", 
        "B", 
        "C"
    };


    scale generateScale() {

        scale m;
        m.classname = "bohlenppierce";
        m.name = "Bohlen Pierce";
        m.description = "The Bohlen Pierce scale is derived from 13 divisions of a 'tritave' - a frequency ratio of 3:1, compared to the octave ratio of 2:1. Here the scale consists of various intervals taken from an justly intonation scale.";
        m.scalename = {
            "C, E, H, A; C1-",
            "C, E, H, Bb; C1-",
            "C, F, G, A; C1-",
            "C, F, H, J; C1-",
            "C, F, H, A; C1-",
            "C, F, H, Bb; C1-",
            "C, G, H, A; C1-",
            "C, G, H, Bb; C1-",
            "C, G, A, Bb; C1-",
            "C, G, Jb, B; C1-",
            "C, Gb, J, B; C1-"
        };

        double n_intervals[NUM_SCALES][3] = { 
            { BP_intervals[3], BP_intervals[7],  BP_intervals[10] },
            { BP_intervals[3], BP_intervals[7],  BP_intervals[11] },
            { BP_intervals[4], BP_intervals[6],  BP_intervals[10] },
            { BP_intervals[4], BP_intervals[7],  BP_intervals[9] },
            { BP_intervals[4], BP_intervals[7],  BP_intervals[10] },
            { BP_intervals[4], BP_intervals[7],  BP_intervals[11] },
            { BP_intervals[6], BP_intervals[7],  BP_intervals[10] },
            { BP_intervals[6], BP_intervals[7],  BP_intervals[11] },
            { BP_intervals[6], BP_intervals[10], BP_intervals[11] },
            { BP_intervals[6], BP_intervals[8],  BP_intervals[12] },
            { BP_intervals[5], BP_intervals[9],  BP_intervals[12] }
        };

        std::string n_intervals_str[NUM_SCALES][3] = { 
            { BP_intervals_str[3], BP_intervals_str[7],  BP_intervals_str[10] },
            { BP_intervals_str[3], BP_intervals_str[7],  BP_intervals_str[11] },
            { BP_intervals_str[4], BP_intervals_str[6],  BP_intervals_str[10] },
            { BP_intervals_str[4], BP_intervals_str[7],  BP_intervals_str[9] },
            { BP_intervals_str[4], BP_intervals_str[7],  BP_intervals_str[10] },
            { BP_intervals_str[4], BP_intervals_str[7],  BP_intervals_str[11] },
            { BP_intervals_str[6], BP_intervals_str[7],  BP_intervals_str[10] },
            { BP_intervals_str[6], BP_intervals_str[7],  BP_intervals_str[11] },
            { BP_intervals_str[6], BP_intervals_str[10], BP_intervals_str[11] },
            { BP_intervals_str[6], BP_intervals_str[8],  BP_intervals_str[12] },
            { BP_intervals_str[5], BP_intervals_str[9],  BP_intervals_str[12] }
        };

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            m.frequency[scaleIdx * NUM_FREQS + 0] = start_freq;
            m.notename[scaleIdx * NUM_FREQS + 0]  = BP_intervals_str[0];

            m.frequency[scaleIdx * NUM_FREQS + 1] = m.frequency[scaleIdx * NUM_FREQS + 0] * n_intervals[scaleIdx][0];
            m.notename[scaleIdx * NUM_FREQS + 1]  = n_intervals_str[scaleIdx][0];

            m.frequency[scaleIdx * NUM_FREQS + 2] = m.frequency[scaleIdx * NUM_FREQS + 0] * n_intervals[scaleIdx][1];
            m.notename[scaleIdx * NUM_FREQS + 2]  = n_intervals_str[scaleIdx][1];

            m.frequency[scaleIdx * NUM_FREQS + 3] = m.frequency[scaleIdx * NUM_FREQS + 0] * n_intervals[scaleIdx][2];
            m.notename[scaleIdx * NUM_FREQS + 3]  = n_intervals_str[scaleIdx][2];

            for (int i = 4; i < 21; i++) {
                m.frequency[scaleIdx * NUM_FREQS + i] = m.frequency[scaleIdx * NUM_FREQS + i - 4] * 3; //tritave
                m.notename[scaleIdx * NUM_FREQS + i]  = m.notename[scaleIdx * NUM_FREQS + i - 4];
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

    bp_generator g = {};
    maxq_filter f = {};
    int i = 1;

    scale s = g.generateScale();

    for (auto freq: s.notename) {
        std::cout << std::to_string(i) << " "<<  freq << std::endl;
        if (i++ % 21 == 0) {
            std::cout << "-- " << std::endl;
        }
        // std::vector<double> c = f.generateCoeffs(freq);
    }

}
