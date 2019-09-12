#include <iostream>
#include <string>
#include <array>
#include <vector>

#include <math.h>

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

struct interval_generator : generator {

    virtual double octave() { return 2.0; }

    void generateFrequencies(scale &m, std::vector<double> n_intervals[NUM_SCALES], std::vector<double> startFrequencies) {

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            for (int noteIdx = 0; noteIdx < n_intervals[scaleIdx].size(); noteIdx++ ) {
                m.frequency[scaleIdx * NUM_FREQS + noteIdx] = n_intervals[scaleIdx][noteIdx] * startFrequencies[scaleIdx];
            }

            for (int i =  n_intervals[scaleIdx].size(); i < NUM_FREQS; i++) {
                m.frequency[scaleIdx * NUM_FREQS + i] = m.frequency[scaleIdx * NUM_FREQS + i - n_intervals[scaleIdx].size()] * octave();
            }
        }
    }

    void generateNames(scale &m, std::vector<std::string> n_intervals_str[NUM_SCALES]) {

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            for (int noteIdx = 0; noteIdx < n_intervals_str[scaleIdx].size(); noteIdx++ ) {
                m.notename[scaleIdx * NUM_FREQS + noteIdx]  = n_intervals_str[scaleIdx][noteIdx];
            }

            for (int i =  n_intervals_str[scaleIdx].size(); i < NUM_FREQS; i++) {
                m.notename[scaleIdx * NUM_FREQS + i]  = m.notename[scaleIdx * NUM_FREQS + i - n_intervals_str[scaleIdx].size()];
            }
        }
    }

};


struct bp_generator : interval_generator {

    virtual double octave() { return 3.0; }

   	double BP_intervals[14] = {
        1.0, 
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
        3.0
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

    std::vector<double> n_intervals[NUM_SCALES] = { 
        { BP_intervals[0], BP_intervals[3], BP_intervals[7],  BP_intervals[10] },
        { BP_intervals[0], BP_intervals[3], BP_intervals[7],  BP_intervals[11] },
        { BP_intervals[0], BP_intervals[4], BP_intervals[6],  BP_intervals[10] },
        { BP_intervals[0], BP_intervals[4], BP_intervals[7],  BP_intervals[9] },
        { BP_intervals[0], BP_intervals[4], BP_intervals[7],  BP_intervals[10] },
        { BP_intervals[0], BP_intervals[4], BP_intervals[7],  BP_intervals[11] },
        { BP_intervals[0], BP_intervals[6], BP_intervals[7],  BP_intervals[10] },
        { BP_intervals[0], BP_intervals[6], BP_intervals[7],  BP_intervals[11] },
        { BP_intervals[0], BP_intervals[6], BP_intervals[10], BP_intervals[11] },
        { BP_intervals[0], BP_intervals[6], BP_intervals[8],  BP_intervals[12] },
        { BP_intervals[0], BP_intervals[5], BP_intervals[9],  BP_intervals[12] }
    };

    std::vector<std::string> n_intervals_str[NUM_SCALES] = { 
        { BP_intervals_str[0], BP_intervals_str[3], BP_intervals_str[7],  BP_intervals_str[10] },
        { BP_intervals_str[0], BP_intervals_str[3], BP_intervals_str[7],  BP_intervals_str[11] },
        { BP_intervals_str[0], BP_intervals_str[4], BP_intervals_str[6],  BP_intervals_str[10] },
        { BP_intervals_str[0], BP_intervals_str[4], BP_intervals_str[7],  BP_intervals_str[9] },
        { BP_intervals_str[0], BP_intervals_str[4], BP_intervals_str[7],  BP_intervals_str[10] },
        { BP_intervals_str[0], BP_intervals_str[4], BP_intervals_str[7],  BP_intervals_str[11] },
        { BP_intervals_str[0], BP_intervals_str[6], BP_intervals_str[7],  BP_intervals_str[10] },
        { BP_intervals_str[0], BP_intervals_str[6], BP_intervals_str[7],  BP_intervals_str[11] },
        { BP_intervals_str[0], BP_intervals_str[6], BP_intervals_str[10], BP_intervals_str[11] },
        { BP_intervals_str[0], BP_intervals_str[6], BP_intervals_str[8],  BP_intervals_str[12] },
        { BP_intervals_str[0], BP_intervals_str[5], BP_intervals_str[9],  BP_intervals_str[12] }
    };


    std::vector<double> startFrequencies {std::vector<double>(11, 32.7031956626)}; // C1

    scale generateScale() {

        scale m;
        m.classname = "bohlenpierce";
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

        generateFrequencies(m, n_intervals, startFrequencies);
        generateNames(m, n_intervals_str);

        return m;

    };

};

struct gamelan_generator : interval_generator {

    scale generateScale() {

        scale m;
        m.classname = "gamelan";
        m.name = "Gamelan Pelog";
        m.description = "Gamelan tunings in C. ";
        m.scalename = {
            "Java (5 notes) low, 5-TET",
            "Bali (7 notes) low, 7-TET",
            "Pelog, var1, low, pathet nem or lima? 7-TET",
            "Pelog, var2, low, pathet nem or lima? 7-TET",
            "Pelog, var3, low, pathet barang? 7-TET",
            "Java (5 note) mid, 5-TET",
            "Bali (7 note) mid, 7-TET",
            "Bali (7 note) high, 7-TET",
            "Pelog, var1, high, pathet nem or lima? 7-TET",
            "Pelog, var2, high, pathet nem or lima? 7-TET",
            "Pelog, var3, high, pathet barang? 7-TET"
        };

        std::vector<double> n_intervals[NUM_SCALES] = { 
            { 1.0, pow(2,1.0/5.0), pow(2,2.0/5.0), pow(2,3.0/5.0), pow(2,4.0/5.0) },                                    // gamelan_5note1 (Java)
            { 1.0, pow(2,1.0/7.0), pow(2,2.0/7.0), pow(2,3.0/7.0), pow(2,4.0/7.0), pow(2,5.0/7.0), pow(2,6.0/7.0) },    // gamelan_7note1 (Bali)
            { 1.0, pow(2,1.0/7.0), pow(2,2.0/7.0), pow(2,4.0/7.0), pow(2,5.0/7.0) },                                    // gamelan_5of7_1 1 2 3 5 6 (Pelog)
            { 1.0, pow(2,1.0/7.0), pow(2,3.0/7.0), pow(2,4.0/7.0), pow(2,6.0/7.0) },                                    // gamelan_5of7_3 1 2 4 5 7 (Pelog)
            { 1.0, pow(2,2.0/7.0), pow(2,3.0/7.0), pow(2,4.0/7.0), pow(2,5.0/7.0) },                                    // gamelan_5of7_5 1 3 4 5 6 (Pelog)
            { 1.0, pow(2,1.0/5.0), pow(2,2.0/5.0), pow(2,3.0/5.0), pow(2,4.0/5.0) },                                    // gamelan_5note2 (Java)
            { 1.0, pow(2,1.0/7.0), pow(2,2.0/7.0), pow(2,3.0/7.0), pow(2,4.0/7.0), pow(2,5.0/7.0), pow(2,6.0/7.0) },    // gamelan_7note2 (Bali)
            { 1.0, pow(2,1.0/7.0), pow(2,2.0/7.0), pow(2,3.0/7.0), pow(2,4.0/7.0), pow(2,5.0/7.0), pow(2,6.0/7.0) },    // gamelan_7note3 (Bali)
            { 1.0, pow(2,1.0/7.0), pow(2,2.0/7.0), pow(2,4.0/7.0), pow(2,5.0/7.0) },                                    // gamelan_5of7_2 (Pelog)
            { 1.0, pow(2,1.0/7.0), pow(2,3.0/7.0), pow(2,4.0/7.0), pow(2,6.0/7.0) },                                    // gamelan_5of7_4 (Pelog)
            { 1.0, pow(2,2.0/7.0), pow(2,3.0/7.0), pow(2,4.0/7.0), pow(2,5.0/7.0) }                                     // gamelan_5of7_6 (Pelog)
        };

        std::vector<std::string> n_intervals_str[NUM_SCALES] = { 
            // { BP_intervals_str[3], BP_intervals_str[7],  BP_intervals_str[10] },
            // { BP_intervals_str[3], BP_intervals_str[7],  BP_intervals_str[11] },
            // { BP_intervals_str[4], BP_intervals_str[6],  BP_intervals_str[10] },
            // { BP_intervals_str[4], BP_intervals_str[7],  BP_intervals_str[9] },
            // { BP_intervals_str[4], BP_intervals_str[7],  BP_intervals_str[10] },
            // { BP_intervals_str[4], BP_intervals_str[7],  BP_intervals_str[11] },
            // { BP_intervals_str[6], BP_intervals_str[7],  BP_intervals_str[10] },
            // { BP_intervals_str[6], BP_intervals_str[7],  BP_intervals_str[11] },
            // { BP_intervals_str[6], BP_intervals_str[10], BP_intervals_str[11] },
            // { BP_intervals_str[6], BP_intervals_str[8],  BP_intervals_str[12] },
            // { BP_intervals_str[5], BP_intervals_str[9],  BP_intervals_str[12] }
        };

        std::vector<double> startFrequencies {
            32.7031956626 + 0.0,            // gamelan_5note1
            32.7031956626 + 2.0,            // gamelan_7note1
            32.7031956626 + 5.0,            // gamelan_5of7_1 1 2 3 5 6
            32.7031956626 + 7.0,            // gamelan_5of7_3 1 2 4 5 7
            32.7031956626 + 9.0,            // gamelan_5of7_5 1 3 4 5 6
            32.7031956626 * 16.0,           // gamelan_5note2
            (32.7031956626 + 3.0) * 8.0,    // gamelan_7note2
            (32.7031956626 + 4.0) * 16.0,   // gamelan_7note3
            (32.7031956626 * 16.0) + 6.0,   // gamelan_5of7_2 
            (32.7031956626 * 16.0) + 8.0,   // gamelan_5of7_4
            (32.7031956626 * 16.0) + 10.0   // gamelan_5of7_6    
        }; // C1
        // To be honest, I really do not understand what 4ms is up to with all these offsets. 
        // Maybe simulating the differences in gamelan tuning?

        generateFrequencies(m, n_intervals, startFrequencies);

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

        return v;

    }

    double calibrateFrequency(double f) {
        return f;
    }

};

int main() {

    gamelan_generator g = {};
    maxq_filter f = {};
    scale s = g.generateScale();

    for (int idx = 0; idx < 231; idx++) {

        int scale = idx / 21;

        if (idx % 21 == 0) {
            std::cout << "-- " << s.scalename[scale] <<  std::endl;
        }

        std::cout << std::to_string(idx) << " " << s.notename[idx] << " " <<  std::to_string(s.frequency[idx]) << std::endl;

        // std::vector<double> c = f.generateCoeffs(freq);
    }

}
