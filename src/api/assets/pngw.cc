#include "pngw.hh"

#include <png.h>
#include <cstring>

PNG::PNG(uint32_t width, uint32_t height, char colorFormat, char bitDepth, std::vector<uint8_t> &&imageData) : width(width), height(height), colorFormat(colorFormat), bitDepth(bitDepth), imageData(imageData){}

void pngErrorCallback(png_structp, png_const_charp error)
{
	printf("%s\n", error);
}

PNG decodePNG(std::string const &filePath)
{
	FILE *input = fopen(filePath.data(), "rb");
	if(!input)
	{
		printf("PNG Decoder: Unable to open %s for reading\n", filePath.data());
		return {0, 0, 0, 0, {}};
	}
	
	png_structp pngPtr;
	png_infop infoPtr;
	png_byte header[8];
	size_t foo = fread(header, 8, 1, input);
	
	if(png_sig_cmp(header, 0, 8) != 0)
	{
		printf("PNG Decoder: File %s is not PNG format\n", filePath.data());
		return {0, 0, 0, 0, {}};
	}
	
	pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, pngErrorCallback, nullptr);
	if(pngPtr == nullptr)
	{
		png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
		printf("PNG Decoder: Failed to create PNG read struct\n");
		return {0, 0, 0, 0, {}};
	}
	
	png_set_error_fn(pngPtr, png_get_error_ptr(pngPtr), [](png_structp, png_const_charp){}, [](png_structp, png_const_charp){});
	infoPtr = png_create_info_struct(pngPtr);
	
	if(!infoPtr)
	{
		png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
		printf("PNG Decoder: Failed to create PNG info struct\n");
		return {0, 0, 0, 0, {}};
	}
	
	if(setjmp(png_jmpbuf(pngPtr)))
	{
		png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
		printf("PNG Decoder: Error during PNG read struct initialization\n");
		return {0, 0, 0, 0, {}};
	}
	
	png_init_io(pngPtr, input);
	png_set_sig_bytes(pngPtr, 8);
	png_read_info(pngPtr, infoPtr);
	uint32_t width, height;
	char colorType, bitDepth;
	width = png_get_image_width(pngPtr, infoPtr);
	height = png_get_image_height(pngPtr, infoPtr);
	colorType = png_get_color_type(pngPtr, infoPtr);
	bitDepth = png_get_bit_depth(pngPtr, infoPtr);
	
	if(colorType & PNG_COLOR_MASK_PALETTE)
	{
		printf("PNG Decoder: Paletted PNG files are not currently supported\n");
		return {0, 0, 0, 0, {}};
	}
	
	png_set_interlace_handling(pngPtr);
	png_read_update_info(pngPtr, infoPtr);
	
	if(setjmp(png_jmpbuf(pngPtr)))
	{
		png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
		printf("PNG Decoder: Failed to read PNG\n");
		return {0, 0, 0, 0, {}};
	}
	std::vector<uint8_t> imageData;
	imageData.resize(height * png_get_rowbytes(pngPtr, infoPtr));
	uint8_t **fauxData = new uint8_t*[height];
	for(size_t i = 0; i < height; i++) fauxData[i] = imageData.data() + i * png_get_rowbytes(pngPtr, infoPtr);
	png_read_image(pngPtr, fauxData);
	png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
	delete [] fauxData;
	fclose(input);
	return PNG{width, height, colorType, bitDepth, std::move(imageData)};
}

struct PNGReadStruct
{
	PNGReadStruct(uint8_t const *data, size_t bufferLocation) : data(data), bufferLocation(bufferLocation) {}
	
	uint8_t const *data = nullptr;
	size_t bufferLocation = 0;
};

void pngReadFn(png_structp read, png_bytep data, png_size_t length)
{
	PNGReadStruct *buffer = reinterpret_cast<PNGReadStruct*>(png_get_io_ptr(read));
	memcpy(data, buffer->data + buffer->bufferLocation, length);
	buffer->bufferLocation += length;
}

PNG decodePNG(std::vector<uint8_t> const &file)
{
	PNGReadStruct pngrs{file.data(), 0};
	png_structp pngPtr;
	png_infop infoPtr;
	png_byte header[8];
	memcpy(header, pngrs.data, 8);
	
	if(png_sig_cmp(header, 0, 8) != 0)
	{
		printf("PNG Decoder: File is not PNG format\n");
		return {0, 0, 0, 0, {}};
	}
	
	pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, pngErrorCallback, nullptr);
	if(pngPtr == nullptr)
	{
		png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
		printf("PNG Decoder: Failed to create PNG read struct\n");
		return {0, 0, 0, 0, {}};
	}
	
	png_set_error_fn(pngPtr, png_get_error_ptr(pngPtr), [](png_structp, png_const_charp){}, [](png_structp, png_const_charp){});
	infoPtr = png_create_info_struct(pngPtr);
	
	if(!infoPtr)
	{
		png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
		printf("PNG Decoder: Failed to create PNG info struct\n");
		return {0, 0, 0, 0, {}};
	}
	
	if(setjmp(png_jmpbuf(pngPtr)))
	{
		png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
		printf("PNG Decoder: Error during PNG read struct initialization\n");
		return {0, 0, 0, 0, {}};
	}
	
	png_set_read_fn(pngPtr, reinterpret_cast<png_voidp *>(&pngrs), pngReadFn);
	png_set_sig_bytes(pngPtr, 0);
	png_read_info(pngPtr, infoPtr);
	uint32_t width, height;
	char colorType, bitDepth;
	width = png_get_image_width(pngPtr, infoPtr);
	height = png_get_image_height(pngPtr, infoPtr);
	colorType = png_get_color_type(pngPtr, infoPtr);
	bitDepth = png_get_bit_depth(pngPtr, infoPtr);
	
	if(colorType & PNG_COLOR_MASK_PALETTE)
	{
		printf("PNG Decoder: Paletted PNG files are not currently supported\n");
		return {0, 0, 0, 0, {}};
	}
	
	png_set_interlace_handling(pngPtr);
	png_read_update_info(pngPtr, infoPtr);
	
	if(setjmp(png_jmpbuf(pngPtr)))
	{
		png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
		printf("PNG Decoder: Failed to read PNG\n");
		return {0, 0, 0, 0, {}};
	}
	std::vector<uint8_t> imageData;
	imageData.resize(height * png_get_rowbytes(pngPtr, infoPtr));
	uint8_t **fauxData = new uint8_t*[height];
	for(size_t i = 0; i < height; i++) fauxData[i] = imageData.data() + i * png_get_rowbytes(pngPtr, infoPtr);
	png_read_image(pngPtr, fauxData);
	png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
	delete [] fauxData;
	return PNG{width, height, colorType, bitDepth, std::move(imageData)};
}

void writePNG(std::string const &filePath, uint32_t width, uint32_t height, uint8_t *imageData, int32_t fmt, bool reverseRows)
{
	uint8_t **data = new uint8_t*[height];
	int32_t cpp = 0;
	switch(fmt)
	{
		case PNG::COLOR_FMT_GREY:
			cpp = 1;
			break;
		case PNG::COLOR_FMT_RGB:
			cpp = 3;
			break;
		case PNG::COLOR_FMT_RGBA:
			cpp = 4;
			break;
	}
	if(reverseRows)
	{
		for(ssize_t i = 0; i < height; i++) data[i] = imageData + (height - i - 1) * width * cpp;
	}
	else
	{
		for(size_t i = 0; i < height; i++) data[i] = imageData + i * width * cpp;
	}
	FILE *output = fopen(filePath.data(), "wb");
	if(!output)
	{
		printf("PNG Encoder: Failed to open %s for writing\n", filePath.data());
		return;
	}
	png_structp pngPtr;
	png_infop infoPtr;
	pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if(!pngPtr)
	{
		printf("PNG Encoder: Failed to create PNG write struct\n");
		fclose(output);
		return;
	}
	infoPtr = png_create_info_struct(pngPtr);
	if(!infoPtr)
	{
		printf("PNG Encoder: Failed to create PNG info struct\n");
		fclose(output);
		return;
	}
	if(setjmp((png_jmpbuf(pngPtr))))
	{
		printf("PNG Encoder: An error occured during I/O init\n");
		fclose(output);
		return;
	}
	png_init_io(pngPtr, output);
	if(setjmp(png_jmpbuf(pngPtr)))
	{
		printf("PNG Encoder: An error occured while writing header\n");
		fclose(output);
		return;
	}
	png_set_IHDR(pngPtr,
	             infoPtr,
	             width,
	             height,
	             8,
	             fmt,
	             PNG_INTERLACE_NONE,
	             PNG_COMPRESSION_TYPE_BASE,
	             PNG_FILTER_TYPE_BASE);
	png_write_info(pngPtr, infoPtr);
	if(setjmp(png_jmpbuf(pngPtr)))
	{
		printf("PNG Encoder: An error occured while writing\n");
		fclose(output);
		return;
	}
	png_write_image(pngPtr, data);
	if(setjmp(png_jmpbuf(pngPtr)))
	{
		printf("PNG Encoder: An error occured during end of write\n");
		fclose(output);
		return;
	}
	png_write_end(pngPtr, nullptr);
	fclose(output);
	delete [] data;
}
