#include <iostream>
#include <iomanip>
#include <string>
#include <array>
#include <vector>
#include <utility>
	

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

    void generateFrequencies(scale &m, 
        std::vector<double> n_intervals[NUM_SCALES], 
        std::vector<double> startFrequencies,
        std::vector<double> octaves ) {

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            for (int noteIdx = 0; noteIdx < n_intervals[scaleIdx].size(); noteIdx++ ) {
                m.frequency[scaleIdx * NUM_FREQS + noteIdx] = 
                    n_intervals[scaleIdx][noteIdx] * startFrequencies[scaleIdx];
            }

            for (int i = n_intervals[scaleIdx].size(); i < NUM_FREQS; i++) {
                m.frequency[scaleIdx * NUM_FREQS + i] = 
                    m.frequency[scaleIdx * NUM_FREQS + i - n_intervals[scaleIdx].size()] * octaves[scaleIdx];
            }
        }
    }

    void generateNames(scale &m, std::vector<std::string> n_intervals_str[NUM_SCALES]) {

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            for (int noteIdx = 0; noteIdx < n_intervals_str[scaleIdx].size(); noteIdx++ ) {
                m.notename[scaleIdx * NUM_FREQS + noteIdx]  = n_intervals_str[scaleIdx][noteIdx];
            }

            for (int i = n_intervals_str[scaleIdx].size(); i < NUM_FREQS; i++) {
                m.notename[scaleIdx * NUM_FREQS + i]  = m.notename[scaleIdx * NUM_FREQS + i - n_intervals_str[scaleIdx].size()];
            }
        }
    }
};

struct wcspread_generator : generator {

    virtual double cents() { return 78.0; }

    void generateFrequencies(scale &m, std::pair<int,int> intervalPairs[11], float startFrequency) {

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            m.frequency[scaleIdx * NUM_FREQS] = startFrequency;
            for (int noteIdx = 1; noteIdx < NUM_FREQS; noteIdx = noteIdx + 2) {

                m.frequency[scaleIdx * NUM_FREQS + noteIdx] = 
                    m.frequency[scaleIdx * NUM_FREQS + noteIdx - 1] *
                    pow(2.0, cents() * intervalPairs[scaleIdx].first / 1200.0);

                m.frequency[scaleIdx * NUM_FREQS + noteIdx + 1] = 
                    m.frequency[scaleIdx * NUM_FREQS + noteIdx] *
                    pow(2.0, cents() * intervalPairs[scaleIdx].second / 1200.0);

            }
        }
    }

    void generateNames(scale &m, std::pair<int,int> intervalPairs[11]) {
        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            m.notename[scaleIdx * NUM_FREQS] = "0";

            for (int noteIdx = 1; noteIdx < NUM_FREQS; noteIdx = noteIdx + 2) {
                m.notename[scaleIdx * NUM_FREQS + noteIdx] = "+" + std::to_string(intervalPairs[scaleIdx].first);
                m.notename[scaleIdx * NUM_FREQS + noteIdx + 1] = "+" + std::to_string(intervalPairs[scaleIdx].second);
            }
        }
    }
};

struct video_generator : generator {

	const double start_freq = 59.94;

    scale generateScale() override {
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

    double octave() { return 3.0; }

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

    std::vector<double> octaves {std::vector<double>(11, 3.0)}; // C1

    scale generateScale() override {

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

        generateFrequencies(m, n_intervals, startFrequencies, octaves);
        generateNames(m, n_intervals_str);

        return m;

    };

};

struct gamelan_generator : interval_generator {

    scale generateScale() override {

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

        std::vector<double> octaves {std::vector<double>(11, 2.0)}; // C1

        generateFrequencies(m, n_intervals, startFrequencies, octaves);

        return m;

    };

};

struct b296_generator : generator {

    const double b296_freqs[21] = {20, 40, 60, 80, 100, 150, 250, 350, 500, 630,800,1000,1300,1600,2000,2600,3500,5000,8000,10000,20000};

    scale generateScale() override {
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

	const double start_freq = 16.3515978313; // C0

    scale generateScale() override {
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

struct mesopotamian_generator : interval_generator {

	// Exact Mesopotamian tuning ratios:
	double I    = 1.0;      		// Unison
	double I0   = 256.0/243.0;		// Pythagorean minor second
	double I1   = 9.0/8.0;			// Pythagorean major second
	double I18  = 32.0/27.0;		// Pythagorean minor third
	double I2   = 81.0/64.0;		// Pythagorean major third
	double I3   = 4.0/3.0;			// Pythagorean perfect fourth
	double I4   = 1024.0/729.0;		// Pythagorean diminished fifth
	double I5   = 3.0/2.0; 			// Pythagorean perfect fifth
	double I58  = 128.0/81.0;		// Pythagorean minor sixth
	double I6   = 27.0/16.0;		// Pythagorean major sixth
	double I7   = 16.0/9.0;			// Pythagorean minor seventh
	double I8   = 243.0/128.0;		// Pythagorean major seventh
	// No Augmented fourth 729:512

    scale generateScale() override {

        scale m;
        m.classname = "mesopotamian";
        m.name = "Mesopotamian";
        m.description = "The Mesopotamian tuning systems have reconstructed from cuneiform tablet from the Sumerian civilisation describing a Babylonian harp. The original work was made in 1960-70s by Duchesne-Guillemin, Kilmer, Gurney and Wulstan deriving a heptatonic ascending scale. Subsequent work by Vitale and Dumbrill indicated that a descending scale was used. The reader is referred to 'NEW LIGHT ON THE BABYLONIAN TONAL SYSTEM, Leon Crickmore for an up-to-date summary of the discussion. Here the module used a Pythagorian tuning and step size suggested by West in THE BABYLONIAN MUSICAL NOTATION AND THE HURRIAN MELODIC TEXTS, p164. The frequencies have been recalculated using more accurate frequency ratios.";
        m.scalename = {
            "Ishartum; A1-",
            "Ishartum; A4-",
            "Embulum; A1-",
            "Embulum; A4-",
            "Nid Murub; A1-",
            "Nid Murub; A4-",
            "Quablitum; A1-",
            "Quablitum; A4-",
            "Kitmun; A1-",
            "Kitmun; A4-",
            "Mitum; A1-"
        };

        std::vector<double> n_intervals[NUM_SCALES] = {
            { I, I0, I18, I3, I5, I58, I7 },   // Ishartum
            { I, I0, I18, I3, I5, I58, I7 },   // Ishartum
            { I, I1, I18, I3, I5, I6, I7 },    // Embulum
            { I, I1, I18, I3, I5, I6, I7 },    // Embulum
            { I, I1, I2, I3, I5, I6, I8 },     // Nid Murub
            { I, I1, I2, I3, I5, I6, I8 },     // Nid Murub
            { I, I0, I18, I4, I5, I58, I7 },   // Quablitum
            { I, I0, I18, I4, I5, I58, I7 },   // Quablitum
            { I, I1, I2, I3, I5, I6, I7 },     // Kitmun
            { I, I1, I2, I3, I5, I6, I7 },     // Kitmun
            { I, I1, I2, I4, I5, I58, I7 }     // Mitum
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
            55.0,    
            55.0 * 8.0,
            55.0,    
            55.0 * 8.0,
            55.0,    
            55.0 * 8.0,
            55.0,    
            55.0 * 8.0,
            55.0,    
            55.0 * 8.0,
            55.0}; 

        std::vector<double> octaves {std::vector<double>(11, 2.0)};

        generateFrequencies(m, n_intervals, startFrequencies, octaves);

        return m;

    };

};

struct alphaspread1_generator : wcspread_generator {

	const double start_freq = 20.60172231; // E0

    std::pair<int,int> intervalPairs[11] = {
        { 4, 10 },
        { 5, 10 },
        { 4, 12 },
        { 5, 11 },
        { 6, 10 },
        { 7, 8 },
        { 8, 7 },
        { 9, 7 },
        { 10, 5 },
        { 11, 5 },
        { 12, 4 }
    };

    scale generateScale() override {

        scale m;
        m.classname = "wc_alpha1";
        m.name = "Alpha Spread 1";
        m.description = "Omitting the octave, Wendy Carlos' Alpha scale is a based on a fixed interval size whose multiples approximate justly intonated intervals. Here the step size is 78 cents - 9 divisions of the perfect fifth, or the minor third in four steps. This scale is constructed from ascending pairs of intervals.";
        m.scalename = {
            "4/10; E0-",
            "5/10; E0-",
            "4/12; E0-",
            "5/11; E0-",
            "6/10; E0-",
            "7/8; E0-",
            "8/7; E0-",
            "9/7; E0-",
            "10/5; E0-",
            "11/5; E0-",
            "12/4; E0-"
        };

        generateFrequencies(m, intervalPairs, start_freq);
        generateNames(m, intervalPairs);

        return m;

    };

};

struct alphaspread2_generator : wcspread_generator {

	const double start_freq = 82.4068892282; // E2

    std::pair<int,int> intervalPairs[11] = {
        { 1, 11 },
        { 2, 10 },
        { 3, 9 },
        { 4, 8 },
        { 5, 7 },
        { 6, 6 },
        { 7, 5 },
        { 8, 4 },
        { 9, 3 },
        { 10, 2 },
        { 11, 1 }
    };

    scale generateScale() override {

        scale m;
        m.classname = "wc_alpha2";
        m.name = "Alpha Spread 2";
        m.description = "Omitting the octave, Wendy Carlos' Alpha scale is a based on a fixed interval size whose multiples approximate justly intonated intervals. Here the step size is 78 cents - 9 divisions of the perfect fifth, or the minor third in four steps. This scale is constructed from ascending pairs of intervals.";
        m.scalename = {
            "1/11; E2-",
            "10/2; E2-",
            "3/9; E2-",
            "4/8; E2-",
            "5/7; E2-",
            "6/6; E2-",
            "7/5; E2-",
            "8/4; E2-",
            "9/3; E2-",
            "10/2; E2-",
            "11/1; E2-"
        };

        generateFrequencies(m, intervalPairs, start_freq);
        generateNames(m, intervalPairs);

        return m;

    };

};

struct gammaspread_generator : wcspread_generator {

	const double start_freq = 20.60172231; // E0

    virtual double cents() override { return 35.099; }

    std::pair<int,int> intervalPairs[11] = {
        { 3, 30 },
        { 5, 29 },
        { 8, 25 },
        { 11, 23 },
        { 13, 21 },
        { 15, 19 },
        { 18, 16 },
        { 20, 14 },
        { 24, 10 },
        { 29, 5 },
        { 31, 3 }
    };

    scale generateScale() override {

        scale m;
        m.classname = "wc_gamma";
        m.name = "Gamma Spread";
        m.description = "Like the Alpha scale, Wendy Carlos' Gamma scale is based on a fixed interval size whose multiples approximate justly intonated intervals. Here the step size is 35 cents; 20 divisions of the perfect fifth. This scale consists of ascending pairs of intervals.";
        m.scalename = {
            "3/30; E0-",
            "5/29; E0-",
            "9/25; E0-",
            "11/23; E0-",
            "13/21; E0-",
            "15/19; E0-",
            "18/16; E0-",
            "20/14; E0-",
            "24/10; E0-",
            "29/5; E0-",
            "31/3; E0-"
        };

        generateFrequencies(m, intervalPairs, start_freq);
        generateNames(m, intervalPairs);

        return m;

    };

};

struct gamma_generator : generator {

	double start_freq = 120.0;
    double cents = 35.099;

    scale generateScale() override {
        scale m;
        m.classname = "gamma_notused";
        m.name = "Gamma";
        m.description = "Like the Alpha scale, Wendy Carlos' Gamma scale is based on a fixed interval size whose multiples approximate justly intonated intervals. Here the step size is 35 cents which is 20 divisions of the perfect fifth. The notes are consequetively arranged starting at 120Hz. This scale is not used in SMR";
        m.scalename = {
            "Gamma 0; 120Hz-",
            "Gamma 1; 180Hz-",
            "Gamma 2; 270Hz-",
            "Gamma 3; 405Hz-",
            "Gamma 4; 607.5Hz-",
            "Gamma 5; 911.3Hz-",
            "Gamma 6; 1367.0HZ-",
            "Gamma 7; 2050.5Hz-",
            "Gamma 8; 3075.8Hz-",
            "Gamma 9; 4613.8Hz-",
            "Gamma 10; 6920.8Hz-"
        };

        double freq = start_freq;

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            m.frequency[scaleIdx * NUM_FREQS] = freq;

            for (int noteIdx = 1; noteIdx < NUM_FREQS; noteIdx++) {
                m.frequency[scaleIdx * NUM_FREQS + noteIdx] = 
                    m.frequency[scaleIdx * NUM_FREQS + noteIdx - 1] * pow(2.0, cents/1200.0);
            }

            freq = m.frequency[scaleIdx * NUM_FREQS + NUM_FREQS - 1];
        }

        return m;

    }

};

struct et17_generator : generator {

	const double start_freq = 13.75;

    scale generateScale() override {
        scale m;
        m.classname = "seventeen";
        m.name = "17 TET";
        m.description = "17 notes per octave, Equal temperament. Scales are arranged consecutively from 13.75Hz to 20kHz";
        m.scalename = {
            "13.75Hz; A-1-",
            "27.5Hz; A0-",
            "55Hz; A1-",
            "110Hz; A2-",
            "220Hz; A3-",
            "440Hz; A4-",
            "880Hz; A5-",
            "1760Hz; A6-",
            "3520Hz; A7-",
            "7040Hz; A8-",
            "14080Hz; A9-"
        };

        double freq = start_freq;

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            m.frequency[scaleIdx * NUM_FREQS] = freq;

            for (int noteIdx = 1; noteIdx < NUM_FREQS; noteIdx++) {
                m.frequency[scaleIdx * NUM_FREQS + noteIdx] = 
                    m.frequency[scaleIdx * NUM_FREQS + noteIdx - 1] * pow(2.0, 1.0/17.0);
            }
            freq *= 2.0;
        }

        return m;

    };

};

struct indian_generator : interval_generator {

    scale generateScale() override {

        scale m;
        m.classname = "indian_penta";
        m.name = "Indian Classical";
        m.description = "The Indian Classical scale contains 12 notes, seven of which form the basic notes - Svara - Sa, Ri/Re, Ga, Ma, Pa, Dha, Ni. Unlike the western tradition, the scale uses just intonation and lacks a reference concert pitch, so these notes are similar although not identical in concept to solfege; the pitch of Sa is used as a reference for the other notes in performance. All notes have several acceptable frequency ratios except Sa and Pa (which are always 1:1 and 3:2). The full set of 22 allowed pitches are the Shrutis. The ratios used in SMR co-incide with 'Ptolemy's intense diatonic scale' in increasing octaves of E. Scales 8 to 11 are selections from  Scale 11 is a repeat of Scale 10. SMR describes this scale as pentatonic, which it is not.";
        m.scalename = {
            "Svara; E0",
            "Svara; E1",
            "Svara; E2",
            "Svara; E3",
            "Svara; E4",
            "Svara; E5",
            "Svara; E6",
            "Sa, Ri, ma, Dha; 120Hz",
            "Svara over 2 octaves; 120Hz",
            "Svara over 5 octaves; 20Hz",
            "Svara over 5 octaves; 20Hz"
        };

        std::vector<double> n_intervals[NUM_SCALES] = { 
            { 1.0, (9.0/8.0), (5.0/4.0), (4.0/3.0), (3.0/2.0), (5.0/3.0), (15.0/8.0) },
            { 1.0, (9.0/8.0), (5.0/4.0), (4.0/3.0), (3.0/2.0), (5.0/3.0), (15.0/8.0) },
            { 1.0, (9.0/8.0), (5.0/4.0), (4.0/3.0), (3.0/2.0), (5.0/3.0), (15.0/8.0) },
            { 1.0, (9.0/8.0), (5.0/4.0), (4.0/3.0), (3.0/2.0), (5.0/3.0), (15.0/8.0) },
            { 1.0, (9.0/8.0), (5.0/4.0), (4.0/3.0), (3.0/2.0), (5.0/3.0), (15.0/8.0) },
            { 1.0, (9.0/8.0), (5.0/4.0), (4.0/3.0), (3.0/2.0), (5.0/3.0), (15.0/8.0) },
            { 1.0, (9.0/8.0), (5.0/4.0), (4.0/3.0), (3.0/2.0), (5.0/3.0), (15.0/8.0) },
            { 1.0, (9.0/8.0), (4.0/3.0), (15.0/8.0) },
            { 1.0, (5.0/4.0), (5.0/3.0), (15.0/8.0), (9.0/8.0) * 2.0, (4.0/3.0) * 2.0, (3.0/2.0) * 2.0 }, // x4
            { 1.0, (9.0/8.0), 
                2.0, (5.0/4.0) * 2.0, 
                4.0, (4.0/3.0) * 4.0, 
                8.0, (3.0/2.0) * 8.0, 
                16.0, (5.0,3.0) * 16.0, 
                32.0, (15.0/8.0) * 32.0 }, // x64
            { 1.0, (9.0/8.0), 
                2.0, (5.0/4.0) * 2.0, 
                4.0, (4.0/3.0) * 4.0, 
                8.0, (3.0/2.0) * 8.0, 
                16.0, (5.0,3.0) * 16.0, 
                32.0, (15.0/8.0) * 32.0 } // x64
        };

        std::vector<std::string> n_intervals_str[NUM_SCALES] = { 
            { "Sa", "Ri", "Ga", "ma", "Pa", "Dha", "Ni" },
            { "Sa", "Ri", "Ga", "ma", "Pa", "Dha", "Ni" },
            { "Sa", "Ri", "Ga", "ma", "Pa", "Dha", "Ni" },
            { "Sa", "Ri", "Ga", "ma", "Pa", "Dha", "Ni" },
            { "Sa", "Ri", "Ga", "ma", "Pa", "Dha", "Ni" },
            { "Sa", "Ri", "Ga", "ma", "Pa", "Dha", "Ni" },
            { "Sa", "Ri", "Ga", "ma", "Pa", "Dha", "Ni" },
            { "Sa", "Ri", "ma", "Ni" },
            { "Sa", "Ga", "Dha", "Ni", "Ri^2", "ma^2", "Pa^2" }, // x4
            { "Sa", "Ri", "Sa^2", "Ga^2", "Sa^3", "ma^3", "Sa^4", "Pa^4", "Sa^5", "Dha^5", "Sa^6", "Ni^6" }, // x64
            { "Sa", "Ri", "Sa^2", "Ga^2", "Sa^3", "ma^3", "Sa^4", "Pa^4", "Sa^5", "Dha^5", "Sa^6", "Ni^6" } // x64
        };

        std::vector<double> startFrequencies {
            20.60172231,
            20.60172231 * 2,
            20.60172231 * 4,
            20.60172231 * 8,
            20.60172231 * 16,
            20.60172231 * 32,
            20.60172231 * 64,
            120.0,
            120.0,
            20.0,
            20.0
        }; 

        std::vector<double> octaves {
            2.0,
            2.0,
            2.0,
            2.0,
            2.0,
            2.0,
            2.0,
            2.0,
            4.0,
            64.0,
            64.0
        };

        generateFrequencies(m, n_intervals, startFrequencies, octaves);
        generateNames(m, n_intervals_str);

        return m;

    };

};

struct diatonicjust_generator : interval_generator {

    double rootA = 110.0;

    scale generateScale() override {

        scale m;
        m.classname = "ji_wholestep";
        m.name = "Whole Step (JI)";
        m.description = "Whole Step scales, Just Intonation";
        m.scalename = {
            "A2-B5",
            "A5-B8",
            "B2-C#5",
            "B5-C#8",
            "C2-D5",
            "C5-D8",
            "D2-E5",
            "D5-E8",
            "E2-F#5",
            "E5-F#8",
            "F4-G7"
        };

        std::vector<double> n_intervals[NUM_SCALES] = { 
            { 1.0, 9.0/8.0, 5.0/4.0, 45.0/32.0, 8.0/5.0, 9.0/5.0 },
            { 1.0, 9.0/8.0, 5.0/4.0, 45.0/32.0, 8.0/5.0, 9.0/5.0 },
            { 1.0, 9.0/8.0, 5.0/4.0, 45.0/32.0, 8.0/5.0, 9.0/5.0 },
            { 1.0, 9.0/8.0, 5.0/4.0, 45.0/32.0, 8.0/5.0, 9.0/5.0 },
            { 1.0, 9.0/8.0, 5.0/4.0, 45.0/32.0, 8.0/5.0, 9.0/5.0 },
            { 1.0, 9.0/8.0, 5.0/4.0, 45.0/32.0, 8.0/5.0, 9.0/5.0 },
            { 1.0, 9.0/8.0, 5.0/4.0, 45.0/32.0, 8.0/5.0, 9.0/5.0 },
            { 1.0, 9.0/8.0, 5.0/4.0, 45.0/32.0, 8.0/5.0, 9.0/5.0 },
            { 1.0, 9.0/8.0, 5.0/4.0, 45.0/32.0, 8.0/5.0, 9.0/5.0 },
            { 1.0, 9.0/8.0, 5.0/4.0, 45.0/32.0, 8.0/5.0, 9.0/5.0 },
            { 1.0, 9.0/8.0, 5.0/4.0, 45.0/32.0, 8.0/5.0, 9.0/5.0 }            
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
            rootA,
            rootA * 8.0,
            rootA * pow(2, 2.0/12.0),
            rootA * pow(2, 2.0/12.0) * 8,
            rootA * pow(2, 3.0/12.0),
            rootA * pow(2, 3.0/12.0) * 8,
            rootA * pow(2, 5.0/12.0),
            rootA * pow(2, 5.0/12.0) * 8,
            rootA * pow(2, 7.0/12.0),
            rootA * pow(2, 7.0/12.0) * 8,
            rootA * pow(2, 8.0/12.0),
            rootA * pow(2, 8.0/12.0) * 8
        }; 

        std::vector<double> octaves {std::vector<double>(11, 2.0)}; // C1

        generateFrequencies(m, n_intervals, startFrequencies, octaves);

        return m;

    };

};

struct diatoniceq_generator : generator {

    double rootA = 110.0;

    scale generateScale() override {

        scale m;
        m.classname = "et_wholestep";
        m.name = "Whole Step (ET)";
        m.description = "Whole Step scales, Equal Temperament";
        m.scalename = {
            "A2-B5",
            "A5-B8",
            "B2-C#5",
            "B5-C#8",
            "C2-D5",
            "C5-D8",
            "D2-E5",
            "D5-E8",
            "E2-F#5",
            "E5-F#8",
            "F4-G7"
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
            rootA,
            rootA * 8.0,
            rootA * pow(2, 2.0/12.0),
            rootA * pow(2, 2.0/12.0) * 8,
            rootA * pow(2, 3.0/12.0),
            rootA * pow(2, 3.0/12.0) * 8,
            rootA * pow(2, 5.0/12.0),
            rootA * pow(2, 5.0/12.0) * 8,
            rootA * pow(2, 7.0/12.0),
            rootA * pow(2, 7.0/12.0) * 8,
            rootA * pow(2, 8.0/12.0)
        }; 

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            m.frequency[scaleIdx * NUM_FREQS] = startFrequencies[scaleIdx];

            for (int noteIdx = 1; noteIdx < NUM_FREQS; noteIdx++) {
                m.frequency[scaleIdx * NUM_FREQS + noteIdx] = 
                    m.frequency[scaleIdx * NUM_FREQS + noteIdx - 1] * pow(2.0, 2.0/12.0);
            }

        }

        return m;

    };

};

struct et_chromatic_generator : generator {

    double A = 13.75;
    double baseE = A * pow(2.0, 7.0/12.0) * 4;
    double baseAsharp =  A * pow(2.0, 1.0/12.0) * 8;

    scale generateScale() override {

        scale m;
        m.classname = "et_chromatic";
        m.name = "Whole Step (ET)";
        m.description = "Chromatic Scale, Equal Temperament";
        m.scalename = {
            "E2-C4",
            "A#2-F#4",
            "E3-C5",
            "A#3-F#5",
            "E4-C6",
            "A#4-F#6",
            "E5-C7",
            "A#5-F#7",
            "E6-C8",
            "A#6-F#8",
            "E7-C9"
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
            baseE,  //E2
            baseAsharp, 
            baseE * 2.0, //E3
            baseAsharp * 2.0, 
            baseE * 4.0, //E4
            baseAsharp * 4.0, 
            baseE * 8.0, //E5
            baseAsharp * 8.0, 
            baseE * 16.0, //E6
            baseAsharp * 16.0, 
            baseE * 32.0, // E7
            baseAsharp * 32.0
        }; 

        for (int scaleIdx = 0; scaleIdx < NUM_SCALES; scaleIdx++) {
            m.frequency[scaleIdx * NUM_FREQS] = startFrequencies[scaleIdx];

            for (int noteIdx = 1; noteIdx < NUM_FREQS; noteIdx++) {
                m.frequency[scaleIdx * NUM_FREQS + noteIdx] = 
                    m.frequency[scaleIdx * NUM_FREQS + noteIdx - 1] * pow(2.0, 1.0/12.0);
            }

        }

        return m;

    };

};

struct ji_triad_generator : interval_generator {

    double start_freq = 32.7 * (3.0/2.0); // Perfect fifth above C1

    scale generateScale() override {

        scale m;
        m.classname = "ji_triads";
        m.name = "Triads (JI)";
        m.description = "Western triads in G, Just Intonation. G1 is defined as a perfect fifth above C1=32.7Hz";
        m.scalename = {
            "Unison, M2, P5; G1-D8",
            "Unison, M3, d5; G1-C#8",
            "Unison, m3, M7; G1-D8",
            "Unison, M3, d5; G1-D8",
            "Unison, P4, P5; G1-D8",
            "Unison, M3, M6; G1-E8",
            "Unison, m3, m6; G1-D#8",
            "Unison, m3, M6; G1-E8",
            "Unison, m3, m7; G1-F8",
            "Unison, m3, m7; G1-F8",
            "Unison, P5, M7; G1-F#8"
        };

        std::vector<double> n_intervals[NUM_SCALES] = { 
            { 1.0, 9.0/8.0, 3.0/2.0 },  // M2_5
            { 1.0, 5.0/4.0, 10.0/7.0 }, // M3_b5
            { 1.0, 6.0/5.0, 15.0/8.0 }, // m3_M5
            { 1.0, 5.0/4.0, 3.0/2.0 },  // M3_5
            { 1.0, 6.0/5.0, 10.0/7.0 }, // m3_#5
            { 1.0, 4.0/3.0, 3.0/2.0 },  // 4_5
            { 1.0, 5.0/4.0, 5.0/3.0 },  // M3_M6
            { 1.0, 6.0/5.0, 8.0/5.0 },  // m3_b6
            { 1.0, 5.0/4.0, 8.0/5.0 },  // M3_#5
            { 1.0, 6.0/5.0, 16.0/9.0 }, // m3_m7
            { 1.0, 3.0/2.0, 15.0/8.0 }  // 5_M7     
        };

        std::vector<std::string> n_intervals_str[NUM_SCALES] = { 
            { "Oct", "M2", "P5" },  // M2_5
            { "Oct", "M3", "d5" },  // M3_b5
            { "Oct", "m3", "M7" },  // m3_M5
            { "Oct", "M3", "P5" },  // M3_5
            { "Oct", "m3", "d5"},   // m3_#5
            { "Oct", "P4", "P5" },  // 4_5
            { "Oct", "M3", "M6" },  // M3_M6
            { "Oct", "m3", "m6" },  // m3_b6
            { "Oct", "M3", "m6" },  // M3_#5
            { "Oct", "m3", "m7" },  // m3_m7
            { "Oct", "P5", "M7" }   // 5_M7     
        };

        std::vector<double> octaves {std::vector<double>(11, 2.0)}; 
        std::vector<double> startFrequencies {std::vector<double>(11, start_freq)}; // G1 

        generateFrequencies(m, n_intervals, startFrequencies, octaves);
        generateNames(m, n_intervals_str);

        return m;

    };

};

struct ji_interval_generator : interval_generator {

    double start_freq = 16.35 * (5.0/3.0); // Major sixth above C0

    scale generateScale() override {

        scale m;
        m.classname = "ji_intervals";
        m.name = "Intervals (JI)";
        m.description = "Single western intervals in A, Just Intonation. A0 is defined as a major sixth above C0=16.35Hz";
        m.scalename = {
            "Octave + m2; A0-A8",
            "Octave + M2; A0-A8",
            "Octave + m3; A0-A8",
            "Octave + M2; A0-A8",
            "Octave + P4; A0-A8",
            "Octave + d5; A0-A8",
            "Octave + P5; A0-A8",
            "Octave + m6; A0-A8",
            "Octave + M6; A0-A8",
            "Octave + m7; A0-A8",
            "Octave + M7; A0-A8"
        };

        std::vector<double> n_intervals[NUM_SCALES] = { 
            { 1.0, 16.0/15.0 }, // m2
            { 1.0, 9.0/8.0 },   // M2
            { 1.0, 6.0/5.0 },   // m3
            { 1.0, 5.0/4.0 },   // M3
            { 1.0, 4.0/3.0 },   // P4
            { 1.0, 10.0/7.0 },  // d5
            { 1.0, 3.0/2.0 },   // P5
            { 1.0, 8.0/5.0 },   // m6
            { 1.0, 5.0/3.0 },   // M6
            { 1.0, 16.0/9.0 },  // m7
            { 1.0, 15.0/8.0 }   // M7
        };

        std::vector<std::string> n_intervals_str[NUM_SCALES] = { 
            { "Oct", "m2" },  
            { "Oct", "M2" },  
            { "Oct", "m3" },  
            { "Oct", "M3" },  
            { "Oct", "P4" },  
            { "Oct", "d5" },  
            { "Oct", "P5" },  
            { "Oct", "m6" },  
            { "Oct", "M6" },  
            { "Oct", "m7" },  
            { "Oct", "M7" }      
        };

        std::vector<double> octaves {std::vector<double>(11, 2.0)}; 
        std::vector<double> startFrequencies {std::vector<double>(11, start_freq)}; // A0

        generateFrequencies(m, n_intervals, startFrequencies, octaves);
        generateNames(m, n_intervals_str);

        return m;

    };

};

struct et_triad_generator : interval_generator {

    double a1 = 440.0 / 16.0;
    double start_freq = a1 * (pow(2.0, 10.0/12.0)); // G1 

    scale generateScale() override {

        scale m;
        m.classname = "et_triads";
        m.name = "Triads (ET)";
        m.description = "Western triads in G, Equal Temperament";
        m.scalename = {
            "Unison, M2, P5; G1-D8",
            "Unison, M3, d5; G1-C#8",
            "Unison, m3, P5; G1-D8",
            "Unison, M3, P5; G1-D8",
            "Unison, P4, P5; G1-D8",
            "Unison, M3, M6; G1-E8",
            "Unison, m3, m6; G1-D#8",
            "Unison, m3, M6; G1-E8",
            "Unison, m3, m7; G1-F8",
            "Unison, P5, m7; G1-F8",
            "Unison, P5, M7; G1-F#8"
        };

        std::vector<double> n_intervals[NUM_SCALES] = { 
            { 1.0, pow(2.0, 2.0/12.0), pow(2.0, 7.0/12.0) },  // M2_5
            { 1.0, pow(2.0, 4.0/12.0), pow(2.0, 6.0/12.0) }, // M3_b5
            { 1.0, pow(2.0, 3.0/12.0), pow(2.0, 11.0/12.0) }, // m3_M5
            { 1.0, pow(2.0, 4.0/12.0), pow(2.0, 7.0/12.0) },  // M3_5
            { 1.0, pow(2.0, 3.0/12.0), pow(2.0, 6.0/12.0) }, // m3_#5
            { 1.0, pow(2.0, 5.0/12.0), pow(2.0, 7.0/12.0) },  // 4_5
            { 1.0, pow(2.0, 4.0/12.0), pow(2.0, 9.0/12.0) },  // M3_M6
            { 1.0, pow(2.0, 3.0/12.0), pow(2.0, 8.0/12.0) },  // m3_b6
            { 1.0, pow(2.0, 4.0/12.0), pow(2.0, 8.0/12.0) },  // M3_#5
            { 1.0, pow(2.0, 3.0/12.0), pow(2.0, 10.0/12.0) }, // m3_m7
            { 1.0, pow(2.0, 7.0/12.0), pow(2.0, 11.0/12.0) }  // 5_M7     
        };

        std::vector<std::string> n_intervals_str[NUM_SCALES] = { 
            { "Oct", "M2", "P5" },  // M2_5
            { "Oct", "M3", "d5" },  // M3_b5
            { "Oct", "m3", "M7" },  // m3_M5
            { "Oct", "M3", "P5" },  // M3_5
            { "Oct", "m3", "d5"},   // m3_#5
            { "Oct", "P4", "P5" },  // 4_5
            { "Oct", "M3", "M6" },  // M3_M6
            { "Oct", "m3", "m6" },  // m3_b6
            { "Oct", "M3", "m6" },  // M3_#5
            { "Oct", "m3", "m7" },  // m3_m7
            { "Oct", "P5", "M7" }   // 5_M7     
        };

        std::vector<double> octaves {std::vector<double>(11, 2.0)}; 
        std::vector<double> startFrequencies {std::vector<double>(11, start_freq)}; // G1 

        generateFrequencies(m, n_intervals, startFrequencies, octaves);
        generateNames(m, n_intervals_str);

        return m;

    };

};

struct et_interval_generator : interval_generator {

    double start_freq = 440.0 / 16.0; // Major sixth above C0

    scale generateScale() override {

        scale m;
        m.classname = "et_intervals";
        m.name = "Intervals (ET)";
        m.description = "Single western intervals in A, Equal Temperament";
        m.scalename = {
            "Octave + m2; A0-A8",
            "Octave + M2; A0-A8",
            "Octave + m3; A0-A8",
            "Octave + M2; A0-A8",
            "Octave + P4; A0-A8",
            "Octave + d5; A0-A8",
            "Octave + P5; A0-A8",
            "Octave + m6; A0-A8",
            "Octave + M6; A0-A8",
            "Octave + m7; A0-A8",
            "Octave + M7; A0-A8"
        };

        std::vector<double> n_intervals[NUM_SCALES] = { 
            { 1.0, pow(2.0, 1.0/12.0) },    // m2
            { 1.0, pow(2.0, 2.0/12.0) },    // M2
            { 1.0, pow(2.0, 3.0/12.0) },    // m3
            { 1.0, pow(2.0, 4.0/12.0) },    // M3
            { 1.0, pow(2.0, 5.0/12.0) },    // P4
            { 1.0, pow(2.0, 6.0/12.0) },    // d5
            { 1.0, pow(2.0, 7.0/12.0) },    // P5
            { 1.0, pow(2.0, 8.0/12.0) },    // m6
            { 1.0, pow(2.0, 9.0/12.0) },    // M6
            { 1.0, pow(2.0, 10.0/12.0) },   // m7
            { 1.0, pow(2.0, 11.0/12.0) }    // M7
        };

        std::vector<std::string> n_intervals_str[NUM_SCALES] = { 
            { "Oct", "m2" },  
            { "Oct", "M2" },  
            { "Oct", "m3" },  
            { "Oct", "M3" },  
            { "Oct", "P4" },  
            { "Oct", "d5" },  
            { "Oct", "P5" },  
            { "Oct", "m6" },  
            { "Oct", "M6" },  
            { "Oct", "m7" },  
            { "Oct", "M7" }      
        };

        std::vector<double> octaves {std::vector<double>(11, 2.0)}; 
        std::vector<double> startFrequencies {std::vector<double>(11, start_freq)}; // A0

        generateFrequencies(m, n_intervals, startFrequencies, octaves);
        generateNames(m, n_intervals_str);

        return m;

    };

};

struct et_major_generator : interval_generator {

    double a0 = 440.0 / 16.0;
    double c1 = a0 * (pow(2.0, 3.0/12.0)); // C1
    double b3 = a0 * 8.0 * (pow(2.0, 2.0/12.0)); // b3
    double a6 = a0 * 64.0;

    scale generateScale() override {

        scale m;
        m.classname = "et_major";
        m.name = "Major (ET)";
        m.description = "Scales from C Major";
        m.scalename = {
            "Major chords; C1-",
            "Major 6th chords; C2-",
            "Major 7th chords; C2-",
            "Augmented chords; C1-",
            "Augmented 7th chords; C2-",
            "Dominant 7th chords; C2-",
            "Major Pentatonic; C1-",
            "Major Pentatonic; C5-",
            "Major scale; C1-",
            "Major scale; C3-",
            "Major scale; C6-"
        };

        std::vector<double> n_intervals[NUM_SCALES] = { 
            { 1.0, pow(2.0, 4.0/12.0), pow(2.0, 7.0/12.0) },                        // Major:           O, M3, P5
            { 1.0, pow(2.0, 4.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 9.0/12.0) },    // Major 6:         O, M3, P5, M6
            { 1.0, pow(2.0, 4.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 11.0/12.0) },   // Major 7:         O, M3, P5, M7
            { 1.0, pow(2.0, 4.0/12.0), pow(2.0, 8.0/12.0) },                        // Augmented:       O, M3, m6
            { 1.0, pow(2.0, 4.0/12.0), pow(2.0, 8.0/12.0), pow(2.0, 11.0/12.0) },   // Augmented 7th:   O, M3, m6, M7
            { 1.0, pow(2.0, 4.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 10.0/12.0) },   // Dominant 7th:    O, M3, P5, m7
            { 1.0, pow(2.0, 2.0/12.0), pow(2.0, 4.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 9.0/12.0) },   // Major Pen:  O, M2, M3, P5, M6
            { 1.0, pow(2.0, 2.0/12.0), pow(2.0, 4.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 9.0/12.0) },   // Major Pen:  O, M2, M3, P5, M6
            { 1.0, pow(2.0, 2.0/12.0), pow(2.0, 4.0/12.0), pow(2.0, 5.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 9.0/12.0), pow(2.0, 11.0/12.0) },   // Major:  O, M2, M3, P4, P5, M6, M7
            { 1.0, pow(2.0, 1.0/12.0), pow(2.0, 3.0/12.0), pow(2.0, 5.0/12.0), pow(2.0, 6.0/12.0), pow(2.0, 8.0/12.0), pow(2.0, 10.0/12.0) },   // Major:  Modified for overall with previous
            { 1.0, pow(2.0, 2.0/12.0), pow(2.0, 3.0/12.0), pow(2.0, 5.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 8.0/12.0), pow(2.0, 10.0/12.0) },   // Major:  Modified for overall with previous
        };

        std::vector<std::string> n_intervals_str[NUM_SCALES] = { 
            { "Oct", "M3", "P5" },  
            { "Oct", "M3", "P5", "M6" },
            { "Oct", "M3", "P5", "M7" },
            { "Oct", "M3", "m6" },  
            { "Oct", "M3", "m5", "M7" },  
            { "Oct", "M3", "P5", "M7" },  
            { "Oct", "M2", "M3", "P5", "M6" },  
            { "Oct", "M2", "M3", "P5", "M6" },  
            { "Oct", "M2", "M3", "P4", "P5", "M6", "M7" },  
            { "M7", "Oct", "M2", "M3", "P4", "P5", "M6"},  
            { "M6", "M7", "Oct", "M2", "M3", "P4", "P5"},  
        };

        std::vector<double> octaves {std::vector<double>(11, 2.0)}; 
        std::vector<double> startFrequencies {
            c1,         // C1
            c1 * 2.0,   // C2
            c1 * 2.0,   // C2
            c1,         // C1
            c1 * 2.0,   // C2
            c1 * 2.0,   // C2
            c1,         // C1
            c1 * 16.0,  // C5
            c1,         // C1
            b3,         // B3
            a6          // A6
        }; 

        generateFrequencies(m, n_intervals, startFrequencies, octaves);
        generateNames(m, n_intervals_str);

        return m;

    };

};

struct et_minor_generator : interval_generator {

    double a0 = 440.0 / 16.0;
    double c1 = a0 * (pow(2.0, 3.0/12.0)); // C1
    double b3 = a0 * 8.0 * (pow(2.0, 2.0/12.0)); // b3
    double f4 = a0 * 8.0 * (pow(2.0, 8.0/12.0)); // f4
    double gs6 = a0 * 32.0 * (pow(2.0, 11.0/12.0)); // gs6
    
    scale generateScale() override {

        scale m;
        m.classname = "et_minor";
        m.name = "Minor (ET)";
        m.description = "Scales from C Major";
        m.scalename = {
            "Minor chords",
            "Minor 6th chords",
            "Minor 7th chords",
            "Half Diminished 7th chords",
            "Minor Pentatonic; C1-",
            "Minor Pentatonic; C5-",
            "Blues scale; C1-",
            "Blues scale; C4-",
            "Harmonic Minor; C1-",
            "Harmonic Minor; C3-",
            "Harmonic Minor; C6-"
        };

        std::vector<double> n_intervals[NUM_SCALES] = { 
            { 1.0, pow(2.0, 3.0/12.0), pow(2.0, 7.0/12.0) },                        // Minor:           O, m3, P5
            { 1.0, pow(2.0, 3.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 8.0/12.0) },    // Minor 6:         O, m3, P5, m6
            { 1.0, pow(2.0, 3.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 10.0/12.0) },   // Minor 7:         O, m3, P5, m7
            { 1.0, pow(2.0, 3.0/12.0), pow(2.0, 4.0/12.0), pow(2.0, 10.0/12.0) },   // Half Dim 7th:    O, m3, M3, m7
            { 1.0, pow(2.0, 3.0/12.0), pow(2.0, 5.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 10.0/12.0) },  // Minor Pen:  O, m3, P4, P5, m7
            { 1.0, pow(2.0, 3.0/12.0), pow(2.0, 5.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 10.0/12.0) },  // Minor Pen:  O, m3, P4, P5, m7
            { 1.0, pow(2.0, 3.0/12.0), pow(2.0, 5.0/12.0), pow(2.0, 6.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 10.0/12.0) },  // Blues:  O, m3, P4, d5, P5, m7
            { 1.0, pow(2.0, 1.0/12.0), pow(2.0, 2.0/12.0), pow(2.0, 5.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 10.0/12.0) },  // Blues:  O, m3, P4, d5, P5, m7 1OK 2OK
            { 1.0, pow(2.0, 2.0/12.0), pow(2.0, 3.0/12.0), pow(2.0, 5.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 8.0/12.0), pow(2.0, 11.0/12.0) },   // HMinor:  O, M2, m3, P4, P5, m6, M7
            { 1.0, pow(2.0, 1.0/12.0), pow(2.0, 3.0/12.0), pow(2.0, 4.0/12.0), pow(2.0, 6.0/12.0), pow(2.0, 8.0/12.0), pow(2.0, 9.0/12.0) },   // HMinor: Modified for overall with previous
            { 1.0, pow(2.0, 3.0/12.0), pow(2.0, 4.0/12.0), pow(2.0, 6.0/12.0), pow(2.0, 7.0/12.0), pow(2.0, 9.0/12.0), pow(2.0, 11.0/12.0) },   // HMinor: Modified for overall with previous
        };

        std::vector<std::string> n_intervals_str[NUM_SCALES] = { 
            { "Oct", "m3", "P5" },  
            { "Oct", "m3", "P5", "m6" },
            { "Oct", "m3", "P5", "m7" },
            { "Oct", "m3", "M4", "m7" },  
            { "Oct", "m3", "P4", "P4", "m7" },  
            { "Oct", "m3", "P4", "P4", "m7" },  
            { "Oct", "M2", "M3", "P5", "d5", "M6" },  
            { "M3", "P5", "d5", "M6", "Oct", "M2" },  
            { "Oct", "M2", "m3", "P4", "P5", "m6", "M7" },  
            { "M7", "Oct", "M2", "m3", "P4", "P5", "m6" },  
            { "m6", "M7", "Oct", "M2", "m3", "P4", "P5" }
        };

        std::vector<double> octaves {std::vector<double>(11, 2.0)}; 
        std::vector<double> startFrequencies {
            c1,         // C1
            c1 * 2.0,   // C2
            c1 * 2.0,   // C2
            c1 * 2.0,   // C2
            c1,         // C1
            c1 * 16.0,  // C5
            c1,         // C1
            f4,         // f4
            c1,         // C1
            b3,         // B3
            gs6         // G#6
        }; 

        generateFrequencies(m, n_intervals, startFrequencies, octaves);
        generateNames(m, n_intervals_str);

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
        return "maxq" + std::to_string(sampleRate);
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
        return "bpre" + std::to_string(sampleRate) + "" + std::to_string(Qval) + "" + std::to_string(gain_q);
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

void procCoeff(scale s, filter *filt, int idx, bool isLast) {
    std::vector<double> coeffs = filt->generateCoeffs(s.frequency[idx]);
    if (coeffs.size() == 1) {
        if (isLast) {
            std::cout << "\t\t" << coeffs[0] << std::endl;
        } else {
            std::cout << "\t\t" << coeffs[0] << "," << std::endl;
        }
    } else {
        std::cout << "\t\t{ ";
        for (int cidx = 0; cidx < coeffs.size() - 1; cidx++) {
            std::cout << coeffs[cidx] << ", ";
        }
        if (isLast) {
            std::cout << coeffs[coeffs.size() - 1] << " }";
        } else {
            std::cout << coeffs[coeffs.size() - 1] << " },";
        }
        std::cout << std::endl;
    }
}

void procFilter(scale s, filter *filt) {
    std::cout << std::setprecision(16);
    std::cout << "\t.c_" << filt->name() << " = {" << std::endl;
    for (int idx = 0; idx < 230; idx++) {
        procCoeff(s, filt, idx, false);
    }
    procCoeff(s, filt, 230, true);
    std::cout << "\t}" << std::endl;
}

int main() {

    std::vector<generator *> generators;
    std::vector<filter *> filters;

    video_generator video = {};
    generators.push_back(&video);

    bp_generator bp = {};
    generators.push_back(&bp);

    gamelan_generator gamelan = {};
    generators.push_back(&gamelan);

    b296_generator b296 = {};
    generators.push_back(&b296);

    shrutis_generator shrutis = {};
    generators.push_back(&shrutis);

    mesopotamian_generator mesopotamian = {};
    generators.push_back(&mesopotamian);

    alphaspread1_generator alpha1 = {};
    generators.push_back(&alpha1);

    alphaspread2_generator alpha2 = {};
    generators.push_back(&alpha2);

    gammaspread_generator gamma_spread = {};
    generators.push_back(&gamma_spread);

    gamma_generator gamma = {};
    generators.push_back(&gamma);
    
    et17_generator et17 = {};
    generators.push_back(&et17);

    indian_generator indian = {};
    generators.push_back(&indian);

    diatonicjust_generator dia_just = {};
    generators.push_back(&dia_just);

    diatoniceq_generator dia_et = {};
    generators.push_back(&dia_et);

    et_chromatic_generator et_chromatic = {};
    generators.push_back(&et_chromatic);

    ji_triad_generator ji_triad = {};
    generators.push_back(&ji_triad);

    ji_interval_generator ji_interval = {};
    generators.push_back(&ji_interval);

    et_triad_generator et_triad = {};
    generators.push_back(&et_triad);

    et_interval_generator et_interval = {};
    generators.push_back(&et_interval);

    et_major_generator et_major = {};
    generators.push_back(&et_major);

    et_minor_generator et_minor = {};
    generators.push_back(&et_minor);

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

    std::cout << "#include \"Scales.hpp\"" << std::endl;

    for (auto g: generators) {

        scale s = g->generateScale();

        std::cout << "Scale " << s.classname << " = {" << std::endl;
        std::cout << "\t.name = \"" << s.name << "\"," << std::endl;
        std::cout << "\t.description = \"" << s.description << "\"," << std::endl;
        std::cout << "\t.scalename = {" << std::endl;

        for (int i = 0; i < s.scalename.size() - 1; i++) {
            std::cout << "\t\t\"" << s.scalename[i] << "\"," << std::endl;
        }
        std::cout << "\t\t\"" << s.scalename[s.scalename.size() - 1] << "\"}," << std::endl;

        std::cout << "\t.notedesc = {" << std::endl;
        for (int i = 0; i < s.notename.size() - 1; i++) {
            std::cout << "\t\t\"" << s.notename[i] << "\"," << std::endl;
        }
        std::cout << "\t\t\"" << s.notename[s.notename.size() - 1] << "\"}," << std::endl;

        procFilter(s, &maxq48);
        procFilter(s, &maxq96);
        procFilter(s, &bpreLo48);
        procFilter(s, &bpreLo96);
        procFilter(s, &bpreHi48);
        procFilter(s, &bpreHi96);

        std::cout << "};" << std::endl;

    }

}

