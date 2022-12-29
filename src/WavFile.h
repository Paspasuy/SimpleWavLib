#ifndef WAVFILE_H
#define WAVFILE_H

#include <cstddef>

class WavFile {
    size_t data_size;
    char* data;
    size_t NumChannels;
    size_t SampleRate;
    size_t BitsPerSample;
public:

    WavFile() = delete;
    WavFile(const char* file);
    WavFile& operator=(const WavFile&);

    size_t ByteRate() const;
    size_t BlockAlign() const;

    void writeToFile(const char* filename) const;

    ~WavFile();
};

#endif  // WAVFILE_H
