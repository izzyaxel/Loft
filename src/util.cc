#include "util.hh"
#include "api/assets/pngw.hh"

#include <glad/glad.h>
#include <commons/threadpool.hh>
#include <commons/fileio.hh>
#include <cstring>

ThreadPool threadPool;

void screenshotIOThread(std::string const &folderPath, uint32_t width, uint32_t height, std::vector<uint8_t> pixels)
{
	createDirectory(folderPath); //Create the screenshots directory if it doesn't exist
	std::string fileName = "Screenshot ";
	std::time_t t = std::time(nullptr);
	std::tm tm = *std::localtime(&t);
	char dt[16];
	strftime(dt, 16, "%m-%d-%y %H%M%S", &tm); //Produce a formatted date and time string for the filename
	fileName += dt;
	fileName += ".png";
	writePNG(folderPath + fileName, width, height, pixels.data(), PNG::COLOR_FMT_RGB, true);
}

void writeScreenshot(std::string const &folderPath, uint32_t width, uint32_t height)
{
	std::vector<uint8_t> pixels;
	pixels.resize(width * height * 3); //Preallocate
	glPixelStorei(GL_PACK_ALIGNMENT, 1); //Ensure the pixel data we get from OGL is in the right format
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data()); //Grab the pixels currently in the buffer and store them in the vector
	threadPool.enqueue(screenshotIOThread, folderPath, width, height, pixels); //I/O will cause a hiccup in the framerate if we don't spin it off into a new asynchronous thread 
}
