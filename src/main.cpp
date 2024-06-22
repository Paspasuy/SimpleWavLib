#include "WavFile.h"
#include "shapes.cpp"
#include <iostream>

void createDefaultFile() {
    int freq = 440;
    int mxharm = 1;
    int mxch = 1;
    WavFile t(2000, mxch);
    for (int ch = 0; ch < mxch; ++ch) {
        for (int harm = 1; harm <= mxharm; ++harm) {
            t.addShape(ch, freq * harm, 0.2 / harm, shapes::sine);
        }
    }
    std::string filename = "default";
//    filename += std::to_string(freq);
    filename += ".wav";
    t.writeToFile(filename.c_str());
}

int main(int argc, const char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <file.wav>\n";
        createDefaultFile();
        return 0;
    }
    WavFile t(argv[1]);
    t.applyEcho(1, 300, 1, 0.3);
    t.distort(0, 30);
    t.distort(0, 0.1);
    t.writeToFile("result.wav");
    return 0;
}
