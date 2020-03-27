#include "models.hh"

#include <commons/fileio.hh>
#include <cstring>

char constexpr const magic[] = {'M', 'S', 'H'};
size_t constexpr const magicSize = 3;

std::unique_ptr<MeshFile> MeshFile::open(std::vector<uint8_t> const &data)
{
	std::unique_ptr<MeshFile> out = std::make_unique<MeshFile>();
	size_t offset = 0;
	char magicIn[3];
	uint64_t numToCEntries = 0, offsetToToC = 0;
	memcpy(&magicIn, data.data() + offset, sizeof(magic));
	if(magicIn[0] != magic[0] && magicIn[1] != magic[1] && magicIn[2] != magic[2]) printf("Error parsing model, model file is corrupted or isn't a valid model file\n");
	offset += magicSize;
	memcpy(&numToCEntries, data.data() + offset, sizeof(numToCEntries));
	offset += sizeof(numToCEntries);
	memcpy(&offsetToToC, data.data() + offset, sizeof(offsetToToC));
	offset += sizeof(offsetToToC);
	size_t datasize = offsetToToC - (magicSize + sizeof(numToCEntries) - sizeof(offsetToToC));
	out->data.resize(datasize);
	memcpy(out->data.data(), data.data() + offset, datasize);
	offset = offsetToToC;
	for(size_t i = 0; i < numToCEntries; i++)
	{
		std::shared_ptr<MeshEntry> entry = std::make_shared<MeshEntry>();
		memcpy(&entry->offsetIntoData, data.data() + offset, sizeof(entry->offsetIntoData));
		offset += sizeof(entry->offsetIntoData);
		memcpy(&entry->modelNameLen, data.data() + offset, sizeof(entry->modelNameLen));
		offset += sizeof(entry->modelNameLen);
		entry->modelName.resize(entry->modelNameLen);
		memcpy(entry->modelName.data(), data.data() + offset, entry->modelNameLen);
		offset += entry->modelNameLen;
		memcpy(&entry->numVertElements, data.data() + offset, sizeof(entry->numVertElements));
		offset += sizeof(entry->numVertElements);
		memcpy(&entry->numUVElements, data.data() + offset, sizeof(entry->numUVElements));
		offset += sizeof(entry->numUVElements);
		memcpy(&entry->numNormalElements, data.data() + offset, sizeof(entry->numNormalElements));
		offset += sizeof(entry->numNormalElements);
		out->toc.push_back(entry);
	}
	return out;
}

std::shared_ptr<MeshEntry> MeshFile::find(std::string const &modelName)
{
	for(auto const &entry : this->toc) if(entry->modelName == modelName) return entry;
	return nullptr;
}

MeshData MeshFile::read(std::string const &modelName)
{
	return this->read(this->find(modelName));
}

MeshData MeshFile::read(std::shared_ptr<MeshEntry> const &tocEntry)
{
	MeshData out{};
	out.modelName = tocEntry->modelName;
	out.numVertElements = tocEntry->numVertElements;
	out.numUVElements = tocEntry->numUVElements;
	out.numNormalElements = tocEntry->numNormalElements;
	out.vertElements.resize(out.numVertElements);
	out.uvElements.resize(out.numUVElements);
	out.normalElements.resize(out.numNormalElements);
	
	uint8_t const *dataStart = this->data.data() + tocEntry->offsetIntoData;
	
	size_t offset = 0;
	memcpy(out.vertElements.data(), dataStart, out.vertElements.size() * sizeof(float));
	offset += out.numVertElements * sizeof(float);
	memcpy(out.uvElements.data(), dataStart + offset, out.uvElements.size() * sizeof(float));
	offset += out.numUVElements * sizeof(float);
	memcpy(out.normalElements.data(), dataStart + offset, out.normalElements.size() * sizeof(float));
	return out;
}

void MeshFile::write(std::string const &outPath, std::vector<MeshData> const &modelData)
{
	FILE *out = fopen(outPath.data(), "wb");
	if(!out) throw std::runtime_error("Unable to open " + outPath + " for writing");
	uint64_t numToCEntries = (uint64_t)modelData.size(), offsetToToC = 0;
	writeFile(out, &magic, magicSize);
	writeFile(out, &numToCEntries, sizeof(numToCEntries));
	writeFile(out, &offsetToToC, sizeof(offsetToToC));
	std::vector<MeshEntry> entries;
	size_t curOffset = 0;
	for(auto const &data : modelData)
	{
		MeshEntry entry{};
		entry.offsetIntoData = curOffset;
		entry.modelName = data.modelName;
		entry.modelNameLen = (uint16_t)entry.modelName.length();
		entry.numVertElements = data.vertElements.size();
		entry.numUVElements = data.uvElements.size();
		entry.numNormalElements = data.normalElements.size();
		entries.push_back(entry);
		size_t bytesWritten = 0;
		bytesWritten += writeFile(out, data.vertElements.data(), data.vertElements.size() * sizeof(float));
		bytesWritten += writeFile(out, data.uvElements.data(), data.uvElements.size() * sizeof(float));
		bytesWritten += writeFile(out, data.normalElements.data(), data.normalElements.size() * sizeof(float));
		curOffset += bytesWritten;
	}
	offsetToToC = curOffset + magicSize + sizeof(numToCEntries) + sizeof(offsetToToC);
	fseek(out, magicSize + sizeof(numToCEntries), SEEK_SET);
	writeFile(out, &offsetToToC, sizeof(offsetToToC));
	fseek(out, (long)offsetToToC, SEEK_SET);
	for(auto const &entry : entries)
	{
		writeFile(out, &entry.offsetIntoData, sizeof(entry.offsetIntoData));
		writeFile(out, &entry.modelNameLen, sizeof(entry.modelNameLen));
		writeFile(out, entry.modelName.data(), entry.modelNameLen);
		writeFile(out, &entry.numVertElements, sizeof(entry.numVertElements));
		writeFile(out, &entry.numUVElements, sizeof(entry.numUVElements));
		writeFile(out, &entry.numNormalElements, sizeof(entry.numNormalElements));
	}
	fclose(out);
}

/// Convert a 3D model format to this one using a callback to process the model
/// \param outPath Path to the output file
/// \param conversionFunc A function that reads a model file from disk and converts it to a ModelData struct
void MeshFile::convert(std::string const &outPath, std::function<void(std::vector<MeshData>&)> const &conversionFunc)
{
	std::vector<MeshData> modelData{};
	conversionFunc(modelData);
	this->write(outPath, modelData);
}
