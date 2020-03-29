#include "audio.hh"

#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <commons/dataBuffer.hh>
#include <commons/fileio.hh>
#include <cstring>
#include <cmath>

float dBToVolume(float dB)
{
	return powf(10.0f, 0.05f * dB);
}

float volumeTodB(float vol)
{
	return 20.0f * log10f(vol);
}

AudioInfo decodeAudio(FILE *input, std::string const &filePath)
{
	AudioInfo out{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	out = fromWAV(input, filePath);
	if(out.format == AudioFormat::NONE || out.bitsPerSample == 0 || out.numChannels == 0 || out.sampleRate == 0 || out.samples.empty())
	{
		out = fromOGG(input, filePath);
	}
	/*if(out.format == AudioFormat::NONE || out.bitsPerSample == 0 || out.numChannels == 0 || out.sampleRate == 0 || out.samples.empty())
	{
		out = fromOpus(input, filePath);
	}*/
	if(out.format == AudioFormat::NONE || out.bitsPerSample == 0 || out.numChannels == 0 || out.sampleRate == 0 || out.samples.empty())
	{
		printf("Failed to decode file %s\n", filePath.data());
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	return out;
}

AudioInfo decodeAudio(std::vector<uint8_t> const &input, std::string const &fileName)
{
	AudioInfo out{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	out = fromOGG(input, fileName);
	if(out.format == AudioFormat::NONE || out.bitsPerSample == 0 || out.numChannels == 0 || out.sampleRate == 0 || out.samples.empty())
	{
		out = fromWAV(input, fileName);
	}
	if(out.format == AudioFormat::NONE || out.bitsPerSample == 0 || out.numChannels == 0 || out.sampleRate == 0 || out.samples.empty())
	{
		printf("Failed to decode file %s\n", fileName.data());
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	return out;
}

AudioInfo fromWAV(FILE *input, std::string const &filePath)
{
	//http://soundfile.sapp.org/doc/WaveFormat/
	std::vector<int16_t> output;
	char chunkID[4], waveFormat[4], subchunk1Id[4], subchunk2Id[4];
	int subchunk1Size = 0, audioFormat = 0;
	size_t dataSize = 0;
	int16_t numChannels = 0, bitsPerSample = 0;
	int32_t sampleRate = 0;
	
	readFile(input, chunkID, 4);
	
	if(memcmp("RIFF", chunkID, 4) != 0)
	{
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	
	fseek(input, 4, SEEK_CUR); //Skips ChunkSize
	readFile(input, waveFormat, 4);
	
	if(memcmp("WAVE", waveFormat, 4) != 0)
	{
		printf("WAV Decoder: Malformed wave file, missing format in: %s\n", filePath.data());
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	
	readFile(input, subchunk1Id, 4);
	
	if(memcmp("fmt ", subchunk1Id, 4) != 0)
	{
		printf("WAV Decoder: Malformed wave file, missing subchunk 1 ID in: %s\n", filePath.data());
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	
	readFile(input, reinterpret_cast<uint8_t *>(&subchunk1Size), 4);
	
	if(16 != subchunk1Size)
	{
		printf("WAV Decoder: Malformed wave file, wrong subchunk 1 size in: %s\n", filePath.data());
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	
	readFile(input, reinterpret_cast<uint8_t *>(&audioFormat), 2);
	
	if(1 != audioFormat)
	{
		printf("WAV Decoder: Compression is unsupported, offending file is: %s\n", filePath.data());
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	
	readFile(input, reinterpret_cast<uint8_t *>(&numChannels), 2);
	readFile(input, reinterpret_cast<uint8_t *>(&sampleRate), 4);
	fseek(input, 6, SEEK_CUR); //Skips ByteRate and BlockAlign
	readFile(input, reinterpret_cast<uint8_t *>(&bitsPerSample), 2);
	readFile(input, subchunk2Id, 4);
	
	if(memcmp("data", subchunk2Id, 4) != 0)
	{
		printf("WAV Decoder: Malformed wave file, missing subchunk 2 ID in: %s\n", filePath.data());
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	
	readFile(input, reinterpret_cast<char *>(&dataSize), 4);
	output.resize(dataSize);
	readFile(input, output.data(), dataSize);
	closeFile(input);
	return AudioInfo{output, numChannels, bitsPerSample, sampleRate, AudioFormat::WAVE};
}

AudioInfo fromOGG(FILE *input, std::string const &filePath)
{
	std::vector<int16_t> output;
	ogg_int16_t convbuffer[4096];
	int convsize = 4096;
	int16_t numChannels = 0, bitsPerSample = 0;
	int32_t sampleRate = 0;
	
	ogg_sync_state syncState;
	ogg_stream_state streamState;
	ogg_page page;
	ogg_packet packet;
	vorbis_info vorbisInfo;
	vorbis_comment comment;
	vorbis_dsp_state dspState;
	vorbis_block block;
	size_t bytesRen = 0;
	char *buffer;
	
	ogg_sync_init(&syncState);
	while(true)
	{
		int eos = 0;
		buffer = ogg_sync_buffer(&syncState, 4096);
		bytesRen = readFile(input, buffer, 4096);
		ogg_sync_wrote(&syncState, (long)bytesRen);
		if(ogg_sync_pageout(&syncState, &page) != 1) if(bytesRen < 4096) break;
		ogg_stream_init(&streamState, ogg_page_serialno(&page));
		vorbis_info_init(&vorbisInfo);
		vorbis_comment_init(&comment);
		if(ogg_stream_pagein(&streamState, &page) < 0)
		{
			printf("OGG Decoder: Error reading first page of OGG bitstream in: %s\n", filePath.data());
			return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
		}
		if(ogg_stream_packetout(&streamState, &packet) != 1)
		{
			printf("OGG Decoder: Error reading initial header in: %s\n", filePath.data());
			return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
		}
		if(vorbis_synthesis_headerin(&vorbisInfo, &comment, &packet) < 0)
		{
			printf("OGG Decoder: File does not contain Vorbis audio: %s\n", filePath.data());
			return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
		}
		int i = 0;
		while(i < 2)
		{
			while(i < 2)
			{
				int result = ogg_sync_pageout(&syncState, &page);
				if(result == 0) break;
				if(result == 1)
				{
					ogg_stream_pagein(&streamState, &page);
					while(i < 2)
					{
						result = ogg_stream_packetout(&streamState, &packet);
						if(result == 0) break;
						if(result < 0)
						{
							printf("OGG Decoder: Corrupted secondary header in: %s\n", filePath.data());
							return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
						}
						result = vorbis_synthesis_headerin(&vorbisInfo, &comment, &packet);
						if(result < 0)
						{
							printf("OGG Decoder: Corrupted secondary header in: %s\n", filePath.data());
							return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
						}
						i++;
					}
				}
			}
			buffer = ogg_sync_buffer(&syncState, 4096);
			readFile(input, buffer, 4096);
			if(bytesRen == 0 && i < 2)
			{
				printf("OGG Decoder: End of file reached before finding all Vorbis headers in: %s\n", filePath.data());
				return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
			}
			ogg_sync_wrote(&syncState, (long)bytesRen);
		}
		convsize /= vorbisInfo.channels;
		if(vorbis_synthesis_init(&dspState, &vorbisInfo) == 0)
		{
			vorbis_block_init(&dspState, &block);
			while(!eos)
			{
				while(!eos)
				{
					int result = ogg_sync_pageout(&syncState, &page);
					if(result == 0) break;
					if(result >= 0)
					{
						ogg_stream_pagein(&streamState, &page);
						while(true)
						{
							result = ogg_stream_packetout(&streamState, &packet);
							if(result == 0) break;
							if(result >= 0)
							{
								float **pcm;
								int samples;
								if(vorbis_synthesis(&block, &packet) == 0) vorbis_synthesis_blockin(&dspState, &block);
								
								while((samples = vorbis_synthesis_pcmout(&dspState, &pcm)) > 0)
								{
									int j;
									int bout = (samples < convsize ? samples : convsize);
									
									for(i = 0; i < vorbisInfo.channels; i++)
									{
										ogg_int16_t *ptr = convbuffer + i;
										float *mono = pcm[i];
										for(j = 0; j < bout; j++)
										{
											int val = (int)(floor(mono[j] * 32767.0 + 0.5));
											*ptr = (ogg_int16_t)val;
											ptr += vorbisInfo.channels;
										}
									}
									output.insert(output.end(), reinterpret_cast<uint8_t *>(convbuffer), reinterpret_cast<uint8_t *>(convbuffer) + (bout * vorbisInfo.channels * 2));
									vorbis_synthesis_read(&dspState, bout);
								}
							}
						}
						if(ogg_page_eos(&page)) eos = 1;
					}
				}
				if(!eos)
				{
					buffer = ogg_sync_buffer(&syncState, 4096);
					readFile(input, buffer, 4096);
					ogg_sync_wrote(&syncState, (long)bytesRen);
					if(bytesRen == 0) eos = 1;
				}
			}
			vorbis_block_clear(&block);
			vorbis_dsp_clear(&dspState);
		}
		else
		{
			printf("OGG Decoder: Corrupt header during init in: %s\n", filePath.data());
		}
		bitsPerSample = 16;
		numChannels = (int16_t)vorbisInfo.channels;
		sampleRate = (int32_t)vorbisInfo.rate;
		ogg_stream_clear(&streamState);
		vorbis_comment_clear(&comment);
		vorbis_info_clear(&vorbisInfo);
	}
	ogg_sync_clear(&syncState);
	if(bitsPerSample == 0 || numChannels == 0 || sampleRate == 0)
	{
		printf("OGG Decoder: An error occured while reading Vorbis file: %s\n", filePath.data());
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	fclose(input);
	return AudioInfo{output, numChannels, bitsPerSample, sampleRate, AudioFormat::OGG};
}

/*AudioInfo fromOpus(FILE *input, std::string const &filePath)
{
	AudioInfo out{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	int32_t err = 0;
	OggOpusFile *opFile = op_open_file(input.data(), &err);
	int64_t streamSize = op_pcm_total(opFile, -1), bytesRead = 0;
	out.numChannels = (int16_t)op_channel_count(opFile, -1);
	while(bytesRead <= streamSize)
	{
		int16_t pcm[5760];
		int32_t read = op_read(opFile, pcm, 5760, nullptr);
		if(read == 0) break;
		bytesRead += read;
		out.samples.resize((size_t)read);
		memcpy(&out.samples[out.samples.size() - read], pcm, (size_t)read);
	}
	op_free(opFile);
	if(out.samples.size() != (size_t)streamSize)
	{
		printf("Opus Decoder: Didn't receive the expected amount of data%s\n", fileName.data());
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	out.format = AudioFormat::OPUS;
	out.bitsPerSample = 16;
	out.sampleRate = 48000;
	return out;
}*/

AudioInfo fromWAV(std::vector<uint8_t> const &input, std::string const &fileName)
{
	//http://soundfile.sapp.org/doc/WaveFormat/
	DataBuffer<uint8_t> buf(input);
	std::vector<int16_t> output;
	char chunkID[4], waveFormat[4]{}, subchunk1Id[4]{}, subchunk2Id[4]{};
	int32_t subchunk1Size = 0, audioFormat = 0;
	size_t dataSize = 0;
	int16_t numChannels = 0, bitsPerSample = 0;
	int32_t sampleRate = 0;
	
	buf.read(chunkID, 4);
	
	if(memcmp("RIFF", chunkID, 4) != 0)
	{
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	
	buf.seek(4, SeekPos::CUR); //Skips ChunkSize
	buf.read(waveFormat, 4);
	
	if(memcmp("WAVE", waveFormat, 4) != 0)
	{
		printf("WAV Decoder: Malformed wave file, missing format\n");
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	
	buf.read(subchunk1Id, 4);
	
	if(memcmp("fmt ", subchunk1Id, 4) != 0)
	{
		printf("WAV Decoder: Malformed wave file, missing subchunk 1 ID\n");
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	
	buf.read(&subchunk1Size, 4);
	
	if(16 != subchunk1Size)
	{
		printf("WAV Decoder: Malformed wave file, wrong subchunk 1 size\n");
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	
	buf.read(&audioFormat, 2);
	
	if(1 != audioFormat)
	{
		printf("WAV Decoder: Compression is unsupported, offending file\n");
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	
	buf.read(&numChannels, 2);
	buf.read(&sampleRate, 4);
	buf.seek(6, SeekPos::CUR); //Skips ByteRate and BlockAlign
	buf.read(&bitsPerSample, 2);
	buf.read(subchunk2Id, 4);
	
	if(memcmp("data", subchunk2Id, 4) != 0)
	{
		printf("WAV Decoder: Malformed wave file, missing subchunk 2 ID\n");
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	
	buf.read(&dataSize, 4);
	output.resize(dataSize);
	buf.read(output.data(), dataSize);
	int16_t const *tmp = reinterpret_cast<int16_t const *>(output.data());
	return AudioInfo{std::vector<int16_t>(tmp, tmp + (output.size() / 2)), numChannels, bitsPerSample, sampleRate, AudioFormat::WAVE};
}

AudioInfo fromOGG(std::vector<uint8_t> const &input, std::string const &fileName)
{
	std::vector<uint8_t> output;
	DataBuffer buf(input);
	ogg_int16_t convbuffer[4096];
	int convsize = 4096;
	int32_t sampleRate = 0;
	int16_t numChannels = 0, bitsPerSample = 0;
	size_t bytesRen = 0;
	
	ogg_sync_state syncState;
	ogg_stream_state streamState;
	ogg_page page;
	ogg_packet packet;
	vorbis_info vorbisInfo;
	vorbis_comment comment;
	vorbis_dsp_state dspState;
	vorbis_block block;
	char *buffer;
	
	ogg_sync_init(&syncState);
	while(true)
	{
		int eos = 0;
		buffer = ogg_sync_buffer(&syncState, 4096);
		bytesRen = buf.read(buffer, 4096);
		ogg_sync_wrote(&syncState, (long)bytesRen);
		if(ogg_sync_pageout(&syncState, &page) != 1) if(bytesRen < 4096) break;
		ogg_stream_init(&streamState, ogg_page_serialno(&page));
		vorbis_info_init(&vorbisInfo);
		vorbis_comment_init(&comment);
		if(ogg_stream_pagein(&streamState, &page) < 0)
		{
			//printf("OGG Decoder: Error reading first page of OGG bitstream: %s\n", fileName.data());
			return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
		}
		if(ogg_stream_packetout(&streamState, &packet) != 1)
		{
			//printf("OGG Decoder: Error reading initial header: %s\n", fileName.data());
			return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
		}
		if(vorbis_synthesis_headerin(&vorbisInfo, &comment, &packet) < 0)
		{
			//printf("OGG Decoder: File does not contain Vorbis audio: %s\n", fileName.data());
			return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
		}
		int i = 0;
		while(i < 2)
		{
			while(i < 2)
			{
				int result = ogg_sync_pageout(&syncState, &page);
				if(result == 0) break;
				if(result == 1)
				{
					ogg_stream_pagein(&streamState, &page);
					while(i < 2)
					{
						result = ogg_stream_packetout(&streamState, &packet);
						if(result == 0) break;
						if(result < 0)
						{
							printf("OGG Decoder: Corrupted secondary header: %s\n", fileName.data());
							return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
						}
						result = vorbis_synthesis_headerin(&vorbisInfo, &comment, &packet);
						if(result < 0)
						{
							printf("OGG Decoder: Corrupted secondary header: %s\n", fileName.data());
							return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
						}
						i++;
					}
				}
			}
			buffer = ogg_sync_buffer(&syncState, 4096);
			bytesRen = buf.read(buffer, 4096);
			if(bytesRen == 0 && i < 2)
			{
				printf("OGG Decoder: End of file reached before finding all Vorbis headers: %s\n", fileName.data());
				return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
			}
			ogg_sync_wrote(&syncState, (long)bytesRen);
		}
		convsize /= vorbisInfo.channels;
		if(vorbis_synthesis_init(&dspState, &vorbisInfo) == 0)
		{
			vorbis_block_init(&dspState, &block);
			while(!eos)
			{
				while(!eos)
				{
					int result = ogg_sync_pageout(&syncState, &page);
					if(result == 0) break;
					if(result >= 0)
					{
						ogg_stream_pagein(&streamState, &page);
						while(true)
						{
							result = ogg_stream_packetout(&streamState, &packet);
							if(result == 0) break;
							if(result >= 0)
							{
								float **pcm;
								int samples;
								if(vorbis_synthesis(&block, &packet) == 0) vorbis_synthesis_blockin(&dspState, &block);
								
								while((samples = vorbis_synthesis_pcmout(&dspState, &pcm)) > 0)
								{
									int j;
									int bout = (samples < convsize ? samples : convsize);
									
									for(i = 0; i < vorbisInfo.channels; i++)
									{
										ogg_int16_t *ptr = convbuffer + i;
										float *mono = pcm[i];
										for(j = 0; j < bout; j++)
										{
											int val = (int)(floor(mono[j] * 32767.0 + 0.5));
											*ptr = (ogg_int16_t)val;
											ptr += vorbisInfo.channels;
										}
									}
									output.insert(output.end(), reinterpret_cast<uint8_t *>(convbuffer), reinterpret_cast<uint8_t *>(convbuffer) + (bout * vorbisInfo.channels * 2));
									vorbis_synthesis_read(&dspState, bout);
								}
							}
						}
						if(ogg_page_eos(&page)) eos = 1;
					}
				}
				if(!eos)
				{
					buffer = ogg_sync_buffer(&syncState, 4096);
					bytesRen = buf.read(buffer, 4096);
					ogg_sync_wrote(&syncState, (long)bytesRen);
					if(bytesRen == 0) eos = 1;
				}
			}
			vorbis_block_clear(&block);
			vorbis_dsp_clear(&dspState);
		}
		else
		{
			printf("OGG Decoder: Corrupt header during init: %s\n", fileName.data());
		}
		bitsPerSample = 16;
		numChannels = (int16_t)vorbisInfo.channels;
		sampleRate = (int32_t)vorbisInfo.rate;
		ogg_stream_clear(&streamState);
		vorbis_comment_clear(&comment);
		vorbis_info_clear(&vorbisInfo);
	}
	ogg_sync_clear(&syncState);
	if(bitsPerSample == 0 || numChannels == 0 || sampleRate == 0)
	{
		printf("OGG Decoder: An error occured while reading Vorbis file: %s\n", fileName.data());
		return AudioInfo{std::vector<int16_t>{}, 0, 0, 0, AudioFormat::NONE};
	}
	auto tmp = reinterpret_cast<int16_t *>(output.data());
	return AudioInfo{std::vector<int16_t>{tmp, tmp + (output.size() / 2)}, numChannels, bitsPerSample, sampleRate, AudioFormat::OGG};
}
