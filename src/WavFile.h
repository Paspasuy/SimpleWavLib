#ifndef WAVFILE_H
#define WAVFILE_H

#include <cstddef>

class WavFile {
    size_t NumChannels;
    size_t SampleRate;
    size_t BitsPerSample;
    size_t data_size;
    char* data;
public:

    WavFile() = delete;
    explicit WavFile(const char* file);
    explicit WavFile(size_t ms_duration, size_t channels);
    WavFile& operator=(const WavFile&);

    [[nodiscard]] size_t ByteRate() const;
    [[nodiscard]] size_t BlockAlign() const;

    void writeToFile(const char* filename) const;

    void addSine(size_t channel, float freq = 440, float fade = 0.5);

    void applyEcho(size_t channel, size_t delay = 75, size_t count = 1, float fade = 0.2);
    void distort(size_t channel, float coeff);

    ~WavFile();
private:
    size_t getChannelShift(size_t channel);
};

#endif  // WAVFILE_H
