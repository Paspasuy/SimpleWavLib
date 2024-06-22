#include "WavFile.h"
#include "wavio.cpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cmath>

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

int16_t crop16(int32_t val) {
    return static_cast<int16_t>(std::min(std::max(val, INT16_MIN), INT16_MAX));
}

char crop8(int32_t val) {
    return static_cast<char>(std::min(std::max(val, INT8_MIN), INT8_MAX));
}

void WavFile::applyEcho(size_t channel, size_t delay, size_t count, float fade) {
    float coeff = fade;
    size_t channel_shift = getChannelShift(channel);
    size_t sample_shift = SampleRate * delay / 1000;
    size_t byte_shift = sample_shift * BlockAlign();
    size_t sh = 0;
    for (size_t it = 1; it <= count; ++it) {
        sh += byte_shift;
        char* end = data + sh + channel_shift;
        char* ptr = data + data_size - BlockAlign() + channel_shift;
        if (BitsPerSample == 16) {
            while (ptr != end) {
                int16_t* pp = reinterpret_cast<int16_t*>(ptr);
                *pp = crop16(*pp + static_cast<int32_t>(coeff * *reinterpret_cast<int16_t*>(ptr - sh)));
                ptr -= BlockAlign();
            }
        } else if (BitsPerSample == 8) {
            while (ptr != end) {
                *ptr = crop8(*ptr + static_cast<int16_t>(coeff * *(ptr - sh)));
                ptr -= BlockAlign();
            }
        }
        coeff *= fade;
    }
}

void WavFile::distort(size_t channel, float coeff) {
    size_t channel_shift = getChannelShift(channel);
    char* ptr = data + channel_shift;
    char* end = data + data_size - BlockAlign() + channel_shift;
    if (BitsPerSample == 16) {
        while (ptr != end) {
            int16_t* pp = reinterpret_cast<int16_t*>(ptr);
            *pp = crop16(static_cast<int32_t>(static_cast<int32_t>(*pp) * coeff));
            ptr += BlockAlign();
        }
    } else if (BitsPerSample == 8) {
        while (ptr != end) {
            *ptr = crop8(static_cast<int16_t>(coeff * static_cast<int16_t>(*ptr)));
            ptr += BlockAlign();
        }
    }
}

WavFile::~WavFile() {
    delete[] data;
}

WavFile::WavFile(size_t ms_duration, size_t channels) : NumChannels(channels), SampleRate(44100), BitsPerSample(16) {
    data_size = ms_duration * SampleRate * BlockAlign() / 1000;
    data = new char[data_size];
    memset(data, 0, data_size);
}

void WavFile::addSine(size_t channel, float freq, float fade) {
    size_t channel_shift = getChannelShift(channel);
    float amplitude = fade * (1 << (BitsPerSample - 1));
    char* ptr = data + channel_shift;
    for (size_t i = 0; i < data_size / BlockAlign(); ++i) {
        float x = i * freq / SampleRate * 2 * M_PI;
        float sample_value = sin(x) * amplitude;
        if (BitsPerSample == 16) {
            *reinterpret_cast<int16_t*>(ptr) += crop16(static_cast<int32_t>(sample_value));
        } else if (BitsPerSample == 8) {
            *reinterpret_cast<int8_t*>(ptr) += crop8(static_cast<int16_t>(sample_value));
        }
        ptr += BlockAlign();
    }
}

size_t WavFile::getChannelShift(size_t channel) {
    if (channel >= NumChannels) {
        throw(std::invalid_argument("Invalid channel"));
    }
    return channel * BitsPerSample / 8;
}
