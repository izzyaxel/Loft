#pragma once

#include <string>
#include <vector>

/// Decibels to volume %
[[nodiscard]] float dBToVolume(float dB);

/// Volume % to decibels
[[nodiscard]] float volumeTodB(float vol);

enum struct AudioFormat
{
	NONE = 0,
	OGG = 1,
	OPUS = 2,
	WAVE = 3,
};

struct AudioInfo
{
	AudioInfo() = default;
	AudioInfo(std::vector<int16_t> const &samples, int16_t numChannels, int16_t bitsPerSample, int32_t sampleRate, AudioFormat format) :
			samples(samples), numChannels(numChannels), bitsPerSample(bitsPerSample), sampleRate(sampleRate), format(format) {}
	
	std::vector<int16_t> samples{};
	int16_t numChannels = 0, bitsPerSample = 0;
	int32_t sampleRate = 0;
	AudioFormat format = AudioFormat::NONE;
};

[[nodiscard]] AudioInfo decodeAudio(FILE *input, std::string const &filePath);
[[nodiscard]] AudioInfo decodeAudio(std::vector<uint8_t> const &input, std::string const &fileName);

[[nodiscard]] AudioInfo fromWAV(FILE *input, std::string const &filePath);
[[nodiscard]] AudioInfo fromOGG(FILE *input, std::string const &filePath);
//[[nodiscard]] AudioInfo fromOpus(FILE *input, std::string const &filePath); //TODO need to use libopus directly for this

[[nodiscard]] AudioInfo fromWAV(std::vector<uint8_t> const &input, std::string const &fileName);
[[nodiscard]] AudioInfo fromOGG(std::vector<uint8_t> const &input, std::string const &fileName);
[[nodiscard]] AudioInfo fromOpus(std::vector<uint8_t> const &input, std::string const &fileName);
