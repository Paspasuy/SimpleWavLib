#include "WavFile.h"
#include "wavio.cpp"

#include <algorithm>

WavFile::WavFile(const char* filename) {
    std::ifstream inp;
    inp.open(filename, std::ios::in | std::ios::binary);
    if (!inp) {
        std::cerr << "Couldn't read file: " << filename << std::endl;
        throw std::runtime_error(filename);
    }
    check_str(inp, "RIFF");
    inp.ignore(4);
    check_str(inp, "WAVE");
    check_str(inp, "fmt\x20");
    check_str(inp, {'\x10', '\x00', '\x00', '\x00'});
    check_str(inp, {'\x01', '\x00'});
    NumChannels = read_num(inp, 2);
    SampleRate = read_num(inp, 4);
    inp.ignore(6);
    BitsPerSample = read_num(inp, 2);
//    std::cerr << NumChannels << ' ' << SampleRate << ' ' << BitsPerSample << std::endl;
    std::string tmp; 
    if ((tmp = read_str(inp, 4)) != "data") {
        std::cerr << "Ignored section: " << tmp << " which is ";
        int x = read_num(inp, 4);
        std::cerr << x << " bytes" << std::endl;
        inp.ignore(x);
        check_str(inp, "data");
    }
    data_size = read_num(inp, 4);
    data = new char[data_size];
    inp.read(data, data_size);
    inp.close();
    if (!inp.good()) {
        std::cerr << "Couldn't close file: " << filename << std::endl;
        throw std::runtime_error(filename);
    }
}

WavFile& WavFile::operator=(const WavFile& other) {
    data_size = other.data_size;
    delete[] data;
    data = new char[data_size];
    std::copy(other.data, other.data + data_size, data);
    return *this;
}

size_t WavFile::ByteRate() const {
    return SampleRate * NumChannels * BitsPerSample / 8;
}

size_t WavFile::BlockAlign() const {
    return NumChannels * BitsPerSample / 8;
}

void WavFile::writeToFile(const char* filename) const {
    std::ofstream out;
    out.open(filename, std::ios::out | std::ios::binary);
    if (!out) {
        std::cerr << "Couldn't write file: " << filename << std::endl;
        throw std::runtime_error(filename);
    }
    std::cerr << "writing " << data_size << " bytes" << std::endl;

    size_t ChunkSize = 44 + data_size;
    out << "RIFF";
    write_num(out, ChunkSize, 4);
    out << "WAVE";
    
    out << "fmt\x20";
    write_num(out, 16, 4);
    out << "\x01";
    out << '\x00';
    write_num(out, NumChannels, 2);
    write_num(out, SampleRate, 4);
    write_num(out, ByteRate(), 4);
    write_num(out, BlockAlign(), 2);
    write_num(out, BitsPerSample, 2);

    out << "data";
    write_num(out, data_size, 4);
    out.write(data, data_size);
    
    out.close();
    if (!out.good()) {
        std::cerr << "Couldn't close file: " << filename << std::endl;
        throw std::runtime_error(filename);
    }
} 

WavFile::~WavFile() {
    delete[] data;
}
