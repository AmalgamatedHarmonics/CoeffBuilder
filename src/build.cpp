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

struct bp_generator : generator {

	double start_freq=32.7031956626; //C1
    double octave = 3.0;

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

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            for (int noteIdx = 0; noteIdx < n_intervals[scaleIdx].size(); noteIdx++ ) {
                m.frequency[scaleIdx * NUM_FREQS + noteIdx] = n_intervals[scaleIdx][noteIdx] * start_freq;
                m.notename[scaleIdx * NUM_FREQS + noteIdx]  = n_intervals_str[scaleIdx][noteIdx];
            }

            for (int i =  n_intervals[scaleIdx].size(); i < NUM_FREQS; i++) {
                m.frequency[scaleIdx * NUM_FREQS + i] = m.frequency[scaleIdx * NUM_FREQS + i - n_intervals[scaleIdx].size()] * octave; //tritave
                m.notename[scaleIdx * NUM_FREQS + i]  = m.notename[scaleIdx * NUM_FREQS + i - n_intervals[scaleIdx].size()];
            }
        }

        return m;
    };

};

struct gamelan_generator : generator {

	double start_freq=32.7031956626; //C1

    // if (scale_i<=1){
    //     sprintf(interval_string[0],"gamelan_5note1");
    //     sprintf(interval_string[1],"gamelan_5note2");
    //     freq0=start_freq;
    //     if (scale_i & 1) freq0=freq0 * 16;

    //     freqs[0]=freq0;
    //     freqs[1]=freq0*n_intervals[0][0];
    //     freqs[2]=freq0*n_intervals[0][1];
    //     freqs[3]=freq0*n_intervals[0][2];
    //     freqs[4]=freq0*n_intervals[0][3];
    //     for (i=5;i<21;i++){
    //         freqs[i]=freqs[i-5]*2.0;
    //     }
    // }

    // if (scale_i>1 && scale_i<=4){
    //     sprintf(interval_string[2],"gamelan_7note1");
    //     sprintf(interval_string[3],"gamelan_7note2");
    //     sprintf(interval_string[4],"gamelan_7note3");
    //     freq0=start_freq+scale_i;
    //     if (scale_i==3) freq0=freq0 * 8;
    //     if (scale_i==4) freq0=freq0 * 16;
    //     freqs[0]=freq0;
    //     freqs[1]=freq0*n_intervals[2][0];
    //     freqs[2]=freq0*n_intervals[2][1];
    //     freqs[3]=freq0*n_intervals[2][2];
    //     freqs[4]=freq0*n_intervals[2][3];
    //     freqs[5]=freq0*n_intervals[2][4];
    //     freqs[6]=freq0*n_intervals[2][5];
    //     for (i=7;i<21;i++){
    //         freqs[i]=freqs[i-7]*2.0;
    //     }
    // }

    // if (scale_i>4){ //5 6, 7 8, 9 10
    //     sprintf(interval_string[scale_i],"gamelan_5of7_%d",scale_i-4);
    //     freq0=start_freq;
    //     if (!(scale_i & 1)) freq0=freq0 * 16;

    //     freqs[0]=freq0+scale_i;
    //     freqs[1]=freq0*n_intervals[scale_i][0];
    //     freqs[2]=freq0*n_intervals[scale_i][1];
    //     freqs[3]=freq0*n_intervals[scale_i][2];
    //     freqs[4]=freq0*n_intervals[scale_i][3];
    //     for (i=5;i<21;i++){
    //         freqs[i]=freqs[i-5]*2.0;
    //     }
    // }

    scale generateScale() {

        scale m;
        m.classname = "gamelan";
        m.name = "Gamelan Pelog";
        m.description = "Gamelan tunings in C";
        m.scalename = {
            "Java (5 notes) low",
            "Java (5 notes) mid",
            "Bali (7 notes) low",
            "Bali (7 notes) mid",
            "Bali (7 notes) high",
            "Pelog, var1, low",
            "Pelog, var1, high",
            "Pelog, var2, low",
            "Pelog, var2, high",
            "Pelog, var3, low; pathet barang?", 
            "Pelog, var3, high; pathet barang?"
        };

        std::vector<double> n_intervals[NUM_SCALES] = { 
            { pow(2,1.0/5), pow(2,2.0/5), pow(2,3.0/5), pow(2,4.0/5) },
            { pow(2,1.0/5), pow(2,2.0/5), pow(2,3.0/5), pow(2,4.0/5) },
            { pow(2,1.0/7), pow(2,2.0/7), pow(2,3.0/7), pow(2,4.0/7), pow(2,5.0/7), pow(2,6.0/7) },
            { pow(2,1.0/7), pow(2,2.0/7), pow(2,3.0/7), pow(2,4.0/7), pow(2,5.0/7), pow(2,6.0/7) },
            { pow(2,1.0/7), pow(2,2.0/7), pow(2,3.0/7), pow(2,4.0/7), pow(2,5.0/7), pow(2,6.0/7) },
            { pow(2,1.0/7), pow(2,2.0/7), pow(2,4.0/7), pow(2,5.0/7) }, // 1 2 3 5 6
            { pow(2,1.0/7), pow(2,2.0/7), pow(2,4.0/7), pow(2,5.0/7) },
            { pow(2,1.0/7), pow(2,3.0/7), pow(2,4.0/7), pow(2,6.0/7) }, // 1 2 4 5 7
            { pow(2,1.0/7), pow(2,3.0/7), pow(2,4.0/7), pow(2,6.0/7) },
            { pow(2,2.0/7), pow(2,3.0/7), pow(2,4.0/7), pow(2,5.0/7) }, // 1 3 4 5 6
            { pow(2,2.0/7), pow(2,3.0/7), pow(2,4.0/7), pow(2,5.0/7) }
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

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            m.frequency[scaleIdx * NUM_FREQS + 0] = start_freq;

            m.frequency[scaleIdx * NUM_FREQS + 1] = m.frequency[scaleIdx * NUM_FREQS + 0] * n_intervals[scaleIdx][0];

            m.frequency[scaleIdx * NUM_FREQS + 2] = m.frequency[scaleIdx * NUM_FREQS + 0] * n_intervals[scaleIdx][1];

            m.frequency[scaleIdx * NUM_FREQS + 3] = m.frequency[scaleIdx * NUM_FREQS + 0] * n_intervals[scaleIdx][2];

            for (int i = 4; i < 21; i++) {
                m.frequency[scaleIdx * NUM_FREQS + i] = m.frequency[scaleIdx * NUM_FREQS + i - 4] * 3.0; 
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

    for (int idx = 0; idx < 231; idx++) {

        std::cout << std::to_string(idx) << " " << s.notename[idx] << " " <<  std::to_string(s.frequency[idx]) << std::endl;
        if (i++ % 21 == 0) {
            std::cout << "-- " << std::endl;
        }
        // std::vector<double> c = f.generateCoeffs(freq);
    }

}
