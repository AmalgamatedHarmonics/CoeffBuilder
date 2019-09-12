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
        m.description = "Gamelan tunings in C.";
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

struct b296_generator : generator {

    double b296_freqs[21] = {20, 40, 60, 80, 100, 150, 250, 350, 500, 630,800,1000,1300,1600,2000,2600,3500,5000,8000,10000,20000};

    scale generateScale() {
        scale m;
        m.classname = "buchla296";
        m.name = "Buchla 296 EQ";
        m.description = "Frequencies from the Buchla 296 EQ module. Each scale is shifted up 50 cents from previous scale.";
        m.scalename = {
            "20.000Hz",
            "20.586Hz",
            "21.189Hz",
            "21.810Hz",
            "22.449Hz",
            "23.107Hz",
            "23.794Hz",
            "24.481Hz",
            "25.198Hz",
            "25.937Hz",
            "26.697Hz"
        };

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
    		double start_freq = pow(2, scaleIdx / 24.0); // freq increases with each scale
            for (int noteIdx = 0; noteIdx < NUM_FREQS; noteIdx++) {
                m.frequency[scaleIdx * NUM_FREQS + noteIdx] = start_freq * b296_freqs[noteIdx];
            }
        }

        return m;

    };

};

struct shrutis_generator : generator {

    double shrutis_intervals[21] = {
    	1.0,
		16.0/15.0,
		10.0/9.0,
		9.0/8.0,
		32.0/27.0,
		6.0/5.0,
		5.0/4.0,
		81.0/64.0,
		4.0/3.0,
		27.0/20.0,
		45.0/32.0,
		729.0/512.0,
		3.0/2.0,
		128.0/81.0,
		8.0/5.0,
		5.0/3.0,
		27.0/16.0,
		16.0/9.0,
		9.0/5.0,
		15.0/8.0,
        243.0/128.0
    };

    std::string shrutis_intervals_str[21] = {
    	"Sa",
		"ri",
		"Ri",
		"Ri",
		"ga",
		"ga",
		"Ga",
		"Ga",
		"ma",
		"ma",
		"Ma",
		"Ma",
		"Pa",
		"dha",
		"dha",
		"Dha",
		"Dha",
		"ni",
		"ni",
		"Ni",
        "Ni"
    };


	double start_freq = 16.3515978313; // C0

    scale generateScale() {
        scale m;
        m.classname = "indian_shrutis";
        m.name = "Indian Shrutis";
        m.description = "In Indian classical music, a shruti is smallest interval or pitch that the human ear can detect and a singer or musical instrument can produce. These are 22 pitches in an octave, the most consonant of which form the 7 notes of the basic scale. Since SMR allows only 21 notes in a scale, the first note of Ri (256:243) is omitted.";
        m.scalename = {
            "Shrutis; C0-",
            "Shrutis; C1-",
            "Shrutis; C2-",
            "Shrutis; C3-",
            "Shrutis; C4-",
            "Shrutis; C5-",
            "Shrutis; C6-",
            "Shrutis; C7-",
            "Shrutis; C8-",
            "Shrutis; C9-",
            "Shrutis; C10-"
        };

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            for (int noteIdx = 0; noteIdx < NUM_FREQS; noteIdx++) {
                m.frequency[scaleIdx * NUM_FREQS + noteIdx] = start_freq * shrutis_intervals[noteIdx] * pow(2,scaleIdx);
                m.notename[scaleIdx * NUM_FREQS + noteIdx] =  shrutis_intervals_str[noteIdx];
            }
        }

        return m;

    };

};


struct filter {

    virtual std::string name() = 0;

    double frequencyCut = 20000.0;
    bool frequencyCutEnabled = true;
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

    std::string name() override {
        return "maxq/" + std::to_string(sampleRate);
    }

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

    std::string name() override {
        return "bpre/" + std::to_string(sampleRate) + "/" + std::to_string(Qval) + "/" + std::to_string(gain_q);
    }

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

    std::vector<generator *> generators;
    std::vector<filter *> filters;

    video_generator video = {};
    // generators.push_back(&video);

    bp_generator bp = {};
    // generators.push_back(&bp);

    gamelan_generator gamelan = {};
    // generators.push_back(&gamelan);

    b296_generator b296 = {};
    // generators.push_back(&b296);

    shrutis_generator shrutis = {};
    generators.push_back(&shrutis);

    maxq_filter maxq48 = {};
    maxq48.sampleRate = 48000;
    filters.push_back(&maxq48);

    maxq_filter maxq96 = {};
    maxq96.sampleRate = 96000;
    filters.push_back(&maxq96);

    bpre_filter bpreLo48 = {};
    bpreLo48.gain_q = 2.0;
    bpreLo48.Qval = 2.0;
    bpreLo48.sampleRate = 48000;
    filters.push_back(&bpreLo48);

    bpre_filter bpreLo96 = {};
    bpreLo96.gain_q = 2.0;
    bpreLo96.Qval = 2.0;
    bpreLo96.sampleRate = 96000;
    filters.push_back(&bpreLo96);

    bpre_filter bpreHi48 = {};
    bpreHi48.gain_q = 40.0;
    bpreHi48.Qval = 800.0;
    bpreHi48.sampleRate = 48000;
    filters.push_back(&bpreHi48);

    bpre_filter bpreHi96 = {};
    bpreHi96.gain_q = 40.0;
    bpreHi96.Qval = 800.0;
    bpreHi96.sampleRate = 96000;
    filters.push_back(&bpreHi96);

    for (auto g: generators) {

        scale s = g->generateScale();

        for (int idx = 0; idx < 231; idx++) {

            int scale = idx / 21;

            if (idx % 21 == 0) {
                std::cout << "-- " << s.scalename[scale] <<  std::endl;
            }

            std::cout << "note " << std::to_string(idx) << " " << s.notename[idx] << " " <<  std::to_string(s.frequency[idx]) << std::endl;

            // for (auto filt: filters) {
            //     std::vector<double> coeffs = filt->generateCoeffs(s.frequency[idx]);
            //     std::cout << filt->name();
            //     for (double c: coeffs) {
            //         std::cout << " " << c;
            //     }
            //     std::cout << std::endl;
            // }
        }
    }
}
