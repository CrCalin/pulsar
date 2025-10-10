#ifndef BMP_LIB_HPP_INCLUDED
#define BMP_LIB_HPP_INCLUDED

#include <vector>
#include <string>
#include "utils.hpp"

struct Pixel {
	uint8_t r, g, b;
	Pixel() {}
	Pixel(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};

enum class BMP_error_t
{
	FILE_OPEN_ERROR,
	FILE_READ_ERROR,
	UNSUPPORTED_IMAGE_FORMAT,
	UNSUPPORTED_BMP_FORMAT,
	INVALID_BMP_FORMAT,
	FILE_WRITE_ERROR,
	IMAGE_WRITE_SUCCESS,
	IMAGE_READ_SUCCESS,
};
class Pixels {
private:
	size_t width, height;
	std::vector<std::vector<Pixel>> pixel_buffer;
public:
	Pixels();
	Pixels(size_t, size_t);

	Pixel& operator[](const Position&);
	size_t get_width() const;
	size_t get_height() const;

	BMP_error_t save(const std::string&);
	BMP_error_t read(const std::string&);

	Pixel& pixel_at(int, int);
	Pixel& pixel_at(Position);

	static const size_t KERNEL_SIZE = 5;
	void blur();
};

#endif // BMP_LIB_HPP_INCLUDED
