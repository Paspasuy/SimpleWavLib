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

    void applyEcho(size_t channel, size_t delay = 75, size_t count = 1, float decease = 0.2);

    ~WavFile();
};

#endif  // WAVFILE_H
