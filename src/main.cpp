#include "WavFile.h"
#include <iostream>

int main(int argc, const char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << "<file.wav>\n";
        return 0;
    }
    WavFile t(argv[1]);
    t.writeToFile("result.wav");
    return 0;
}
