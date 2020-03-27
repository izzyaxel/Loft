#include "asa.hh"

#include <commons/fileio.hh>
#include <commons/stringtools.hh>
#include <commons/buffer.hh>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <zstd.h>

std::string const doNotCompress[] = {".ogg", ".oga", ".ogv", ".opus", ".png", ".zip", ".rar", ".7z", ".tar", ".xz", ".asa"};
size_t constexpr magicSize = 3, headerSize = sizeof(Header), offsetToData = magicSize + headerSize;
char const constexpr magic[] = {'A', 'S', 'A'};

// ZSTD Implementation -=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-//
struct Decompressor
{
	using Callback = std::function<void (std::vector<uint8_t>)>;
	Decompressor(Callback const &cb) : callback(cb)
	{
		this->dstream = ZSTD_createDStream();
		this->tempBuf.resize(ZSTD_DStreamOutSize());
		ZSTD_initDStream(this->dstream);
		this->worker.reset(new std::thread {&Decompressor::workerRun, this});
	}
	
	Decompressor(Decompressor const &) = delete;
	Decompressor(Decompressor &&) = delete;
	
	~Decompressor()
	{
		this->workerRunSem.store(false);
		this->workerMtx.lock();
		this->workerCV.notify_all();
		this->workerMtx.unlock();
		this->worker->join();
		ZSTD_freeDStream(this->dstream);
	}
	
	void pushData(uint8_t const *data, size_t len)
	{
		{
			std::lock_guard<std::mutex> lck {this->bufferMtx};
			this->masterBuffer.write(data, len);
		}
		{
			this->workerMtx.lock();
			this->workerCV.notify_all();
			this->workerMtx.unlock();
		}
	}
	
	void workerLoop()
	{
		size_t exp = 0;
		ZSTD_inBuffer inBuffer = {this->workerBuffer.data(), this->workerBuffer.size(), 0};
		do
		{
			ZSTD_outBuffer outBuffer = {this->tempBuf.data(), this->tempBuf.size(), 0};
			exp = ZSTD_decompressStream(this->dstream, &outBuffer, &inBuffer);
			this->callback(std::vector<uint8_t>{reinterpret_cast<uint8_t*>(outBuffer.dst), reinterpret_cast<uint8_t*>(outBuffer.dst) + outBuffer.pos});
		} while(exp < inBuffer.size - inBuffer.pos && inBuffer.pos < inBuffer.size);
	}
	
	void workerRun()
	{
		std::unique_lock<std::mutex> lck {this->workerMtx};
		while(this->workerRunSem)
		{
			this->workerCV.wait(lck);
			do
			{
				this->workerLoop();
			} while([this]()
			{
				std::lock_guard<std::mutex> lckG {this->bufferMtx};
				bool newData = this->masterBuffer.size();
				if(newData) this->masterBuffer.transferTo(this->workerBuffer);
				return newData;
			}());
		}
	}
	
	static size_t recommendedInputSize()
	{
		return ZSTD_DStreamInSize();
	}
	
	std::unique_ptr<std::thread> worker;
	std::condition_variable workerCV;
	std::atomic_bool workerRunSem{true};
	std::mutex workerMtx, bufferMtx;
	ZSTD_DStream *dstream = nullptr;
	Buffer masterBuffer, workerBuffer;
	std::vector<uint8_t> tempBuf;
	Callback callback;
};

std::vector<uint8_t> decompress(std::vector<uint8_t> const &in)
{
	std::vector<uint8_t> out{};
	out.resize(ZSTD_getDecompressedSize(in.data(), in.size()));
	size_t decompressedSize = ZSTD_decompress(out.data(), out.size(), in.data(), in.size());
	if(ZSTD_isError(decompressedSize)) throw std::runtime_error("ZSTD decompression: " + std::string(ZSTD_getErrorName(decompressedSize)));
	out.resize(decompressedSize);
	return out;
}

std::vector<uint8_t> compress(std::vector<uint8_t> &data)
{
	std::vector<uint8_t> out{};
	size_t maxCompressedSize = ZSTD_compressBound(data.size());
	out.resize(maxCompressedSize);
	size_t compressedSize = ZSTD_compress(out.data(), out.size(), data.data(), data.size(), ZSTD_maxCLevel());
	if(ZSTD_isError(compressedSize)) throw std::runtime_error("ZSTD compression: " + std::string(ZSTD_getErrorName(compressedSize)));
	out.resize(compressedSize);
	return out;
}
// -=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-//

ASA::~ASA()
{
	closeFile(this->in);
	delete reinterpret_cast<Decompressor*>(this->decompressor);
}

std::vector<uint8_t> ASA::read(std::string const &fileName)
{
	std::vector<uint8_t> out{};
	auto entry = this->find(fileName);
	if(!entry) return out;
	return this->read(entry);
}

std::vector<uint8_t> ASA::read(std::shared_ptr<ASAEntry> const &entry)
{
	std::vector<uint8_t> out{};
	if(entry->format == cmpFmtNone)
	{
		out.resize(entry->decompressedSize);
		fseek(this->in, (long)(magicSize + headerSize + entry->offset), SEEK_SET);
		readFile(this->in, out.data(), out.size());
	}
	else if(entry->format == cmpFmtZSTD)
	{
		out.resize(entry->decompressedSize);
		#if 1 //Whole file decompression, tested ok
		std::vector<uint8_t> inter{};
		inter.resize(entry->compressedSize);
		fseek(this->in, (long)(magicSize + headerSize + entry->offset), SEEK_SET);
		readFile(this->in, inter.data(), inter.size());
		out = decompress(inter);
		#else //Block decompression, tested failing, data offset is wrong, file is mostly 0s
		this->decompressor = new Decompressor{[&out](std::vector<uint8_t> decompData){ out.insert(out.end(), decompData.begin(), decompData.end()); }};
		std::vector<uint8_t> readBuffer;
		readBuffer.resize(reinterpret_cast<Decompressor *>(this->decompressor)->recommendedInputSize());
		fseek(this->in, (long)(magicSize + headerSize + entry->offset), SEEK_SET);
		while(true)
		{
			size_t bytesRead = fread(readBuffer.data(), 1, readBuffer.size(), this->in);
			reinterpret_cast<Decompressor*>(this->decompressor)->pushData(readBuffer.data(), bytesRead);
			if(bytesRead != readBuffer.size()) break;
		}
		delete reinterpret_cast<Decompressor*>(this->decompressor);
		#endif
	}
	return out;
}

std::unique_ptr<ASA> ASA::open(std::string const &filepath)
{
	std::unique_ptr<ASA> out{new ASA};
	out->in = openFile(filepath, "rb");
	if(!out->in)
	{
		printf("ASA reading: failed to open file: %s\n", filepath.data());
		return {};
	}
	char magicIn[magicSize];
	readFile(out->in, &magicIn, magicSize);
	for(size_t i = 0; i < magicSize; i++) if(magic[i] != magicIn[i]) throw std::runtime_error("ASA parsing failed, this is not an ASA file");
	readFile(out->in, &out->header, headerSize);
	fseek(out->in, (long)out->header.tocBeginOffset, SEEK_SET);
	for(size_t i = 0; i < out->header.numToCEntries; i++)
	{
		ASAEntry entry;
		readFile(out->in, &entry.format, sizeof(entry.format));
		readFile(out->in, &entry.filenameLen, sizeof(entry.filenameLen));
		entry.filename.resize(entry.filenameLen);
		readFile(out->in, entry.filename.data(), entry.filenameLen);
		readFile(out->in, &entry.compressedSize, sizeof(entry.compressedSize));
		readFile(out->in, &entry.decompressedSize, sizeof(entry.decompressedSize));
		readFile(out->in, &entry.offset, sizeof(entry.offset));
		out->toc.push_back(std::make_shared<ASAEntry>(entry));
	}
	fseek(out->in, magicSize + headerSize, SEEK_SET); //seek to start of data
	return out;
}

void ASA::write(std::string const &asaFilePath, std::vector<std::string> const &filePathes)
{
	FILE *out = openFile(asaFilePath, "wb");
	if(!out) throw std::runtime_error("ASA Writing: Failed to open" + asaFilePath + "for writing");
	std::vector<ASAEntry> toc;
	size_t tocEntries = filePathes.size(), totalOffset = 0;
	FILE *in = nullptr;
	ASAEntry tmp{};
	writeFile(out, &magic, magicSize);
	writeFile(out, &tocEntries, sizeof(tocEntries));
	writeFile(out, &totalOffset, sizeof(totalOffset));
	for(auto const &path : filePathes)
	{
		in = openFile(path, "rb");
		if(!in) throw std::runtime_error("ASA Writing: Failed to open asset " + path);
		fseek(in, 0, SEEK_END);
		size_t len = (size_t)ftell(in);
		rewind(in);
		std::vector<uint8_t> uncompressed(len);
		readFile(in, uncompressed.data(), uncompressed.size());
		bool shouldCompress = true;
		for(auto const &dnc : doNotCompress)
		{
			if(endsWith(path, dnc))
			{
				shouldCompress = false;
				break;
			}
		}
		if(shouldCompress)
		{
			std::vector<uint8_t> compressed = compress(uncompressed);
			writeFile(out, compressed.data(), compressed.size());
			tmp.compressedSize = compressed.size();
		}
		else
		{
			writeFile(out, uncompressed.data(), uncompressed.size());
			tmp.compressedSize = uncompressed.size();
		}
		
		tmp.format = shouldCompress ? cmpFmtZSTD : cmpFmtNone;
		tmp.filename = path.substr(path.find_last_of('/') + 1);
		tmp.filenameLen = (uint16_t)tmp.filename.length();
		tmp.decompressedSize = len;
		tmp.offset = totalOffset;
		totalOffset += tmp.compressedSize;
		toc.push_back(tmp);
		tmp = {};
		closeFile(in);
		in = nullptr;
	}
	size_t eofData = (size_t)ftell(out);
	if((eofData - magicSize - headerSize) != totalOffset) throw std::runtime_error("Sanity check failure, data blob length mismatch");
	fseek(out, magicSize + sizeof(tocEntries), SEEK_SET);
	writeFile(out, &eofData, sizeof(eofData));
	fseek(out, (long)eofData, SEEK_SET);
	for(auto const &entry : toc) //Write the ToC
	{
		writeFile(out, &entry.format, sizeof(entry.format));
		writeFile(out, &entry.filenameLen, sizeof(entry.filenameLen));
		writeFile(out, entry.filename.data(), entry.filename.length());
		writeFile(out, &entry.compressedSize, sizeof(entry.compressedSize));
		writeFile(out, &entry.decompressedSize, sizeof(entry.decompressedSize));
		writeFile(out, &entry.offset, sizeof(entry.offset));
	}
	closeFile(out);
}

void ASA::append(std::string const &inputASAFilePath, std::vector<std::string> const &filePathes)
{
	FILE *out = fopen(inputASAFilePath.data(), "r+");
	if(!out) throw std::runtime_error("ASA Appending: Failed to open " + inputASAFilePath + " for appending");
	char magicIn[3];
	uint64_t numToCEntries = 0, tocBeginOffset = 0;
	readFile(out, magicIn, magicSize);
	for(size_t i = 0; i < magicSize; i++) if(magic[i] != magicIn[i]) throw std::runtime_error("ASA Appending: source file has incorrect magic");
	readFile(out, &numToCEntries, sizeof(numToCEntries));
	readFile(out, &tocBeginOffset, sizeof(tocBeginOffset)); //FIXME read fails
	fseek(out, tocBeginOffset, SEEK_SET);
	size_t totalOffset = 0;
	
	std::vector<ASAEntry> toc;
	for(uint64_t i = 0; i < numToCEntries; i++) //capture a copy of the ToC
	{
		ASAEntry entry;
		readFile(out, &entry.format, sizeof(entry.format));
		readFile(out, &entry.filenameLen, sizeof(entry.filenameLen));
		entry.filename.resize(entry.filenameLen);
		readFile(out, entry.filename.data(), entry.filenameLen);
		readFile(out, &entry.compressedSize, sizeof(entry.compressedSize));
		readFile(out, &entry.decompressedSize, sizeof(entry.decompressedSize));
		readFile(out, &entry.offset, sizeof(entry.offset));
		totalOffset += entry.compressedSize;
		toc.push_back(entry);
	}
	if((totalOffset + offsetToData) != tocBeginOffset) throw std::runtime_error("Sanity failed, stored offset to ToC doesn't match calculated offset");
	fseek(out, tocBeginOffset, SEEK_SET);
	FILE *in = nullptr;
	ASAEntry tmp{};
	for(auto const &path : filePathes)
	{
		in = fopen(path.data(), "rb");
		if(!in) throw std::runtime_error("ASA Appending: Failed to open asset " + path);
		fseek(in, 0, SEEK_END);
		size_t len = (size_t)ftell(in);
		rewind(in);
		std::vector<uint8_t> uncompressed(len);
		readFile(in, uncompressed.data(), uncompressed.size());
		bool shouldCompress = true;
		for(auto const &dnc : doNotCompress)
		{
			if(endsWith(path, dnc))
			{
				shouldCompress = false;
				break;
			}
		}
		if(shouldCompress)
		{
			std::vector<uint8_t> compressed = compress(uncompressed);
			writeFile(out, compressed.data(), compressed.size());
			tmp.compressedSize = compressed.size();
		}
		else
		{
			writeFile(out, uncompressed.data(), uncompressed.size());
			tmp.compressedSize = uncompressed.size();
		}
		
		tmp.format = shouldCompress ? cmpFmtZSTD : cmpFmtNone;
		tmp.filename = path.substr(path.find_last_of('/') + 1);
		tmp.filenameLen = (uint16_t)tmp.filename.length();
		tmp.decompressedSize = len;
		tmp.offset = totalOffset;
		totalOffset += tmp.compressedSize;
		printf("%zu %zu\n", (size_t)ftell(out), totalOffset + offsetToData);
		toc.push_back(tmp);
		tmp = {};
		closeFile(in);
		in = nullptr;
	}
	size_t eofData = (size_t)ftell(out);
	if((totalOffset + offsetToData) != eofData) throw std::runtime_error("Sanity failed, calculated offset to ToC doesn't match actual offset to ToC");
	fseek(out, magicSize, SEEK_SET);
	numToCEntries += filePathes.size();
	writeFile(out, &numToCEntries, sizeof(numToCEntries));
	writeFile(out, &eofData, sizeof(eofData));
	
	fseek(out, eofData, SEEK_SET);
	
	for(auto const &entry : toc) //Write the new ToC
	{
		writeFile(out, &entry.format, sizeof(entry.format));
		writeFile(out, &entry.filenameLen, sizeof(entry.filenameLen));
		writeFile(out, entry.filename.data(), entry.filename.length());
		writeFile(out, &entry.compressedSize, sizeof(entry.compressedSize));
		writeFile(out, &entry.decompressedSize, sizeof(entry.decompressedSize));
		writeFile(out, &entry.offset, sizeof(entry.offset));
	}
	closeFile(out);
}

std::shared_ptr<ASAEntry> ASA::find(std::string const &filename)
{
	for(auto const &tocEntry : this->toc) if(tocEntry->filename == filename) return tocEntry;
	return nullptr;
}
