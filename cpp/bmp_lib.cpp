#include <fstream>
#include <memory>
#include "bmp_lib.hpp"
#include "utils.hpp"

Pixels::Pixels() : width(0), height(0) {}

Pixels::Pixels(size_t width, size_t height) : width(width), height(height) {
	pixel_buffer.resize(height);
	for (auto& v : pixel_buffer) {
		v.resize(width);
	}
}

Pixel& Pixels::operator[](const Position& p) {
	return pixel_buffer[p.i][p.j];
}

size_t Pixels::get_width() const {
	return width;
}
size_t Pixels::get_height() const {
	return height;
}

constexpr size_t nearest4(size_t x) {
	return ((x % 4 == 0) ? x : x / 4 * 4 + 4);
}
const int BMP_HEADER_SIZE = 14;
const int DIB_HEADER_SIZE = 40;
BMP_error_t Pixels::save(const std::string& file_name) {
	size_t row_size_on_disk = nearest4(width * 3); // 3 bytes per pixel, padded to nearest 4
	size_t size_of_array = row_size_on_disk * height;

	LEByteBuffer header;
	header.append_i16(0x4D42); // all BMP files begin with 0x4D42 (little endian)
	header.append_i32(size_of_array + BMP_HEADER_SIZE + DIB_HEADER_SIZE); // size of file = size of pixel array + size of headers
	header.append_i16(0); // unused value
	header.append_i16(0); // unused value
	header.append_i32(BMP_HEADER_SIZE + DIB_HEADER_SIZE); // beginning of pixel array

	LEByteBuffer DIB_header;
	DIB_header.append_i32(40); // We are using the standard BITMAPINFOHEADER, so the size is 40 bytes
	DIB_header.append_i32(width); // width of image in px
	DIB_header.append_i32(height); // height of image in px
	DIB_header.append_i16(1); // number of color planes; always 1
	DIB_header.append_i16(24); // Bits per pixel
	DIB_header.append_i32(0); // no compression
	DIB_header.append_i32(size_of_array); // size of raw bitmap data
	DIB_header.append_i32(0x0EC4); // horizontal resolution (in px/m), dummy value i got from paint
	DIB_header.append_i32(0x0EC4); // vertucal resolution (in px/m), dummy value i got from paint
	DIB_header.append_i32(0); // number of colors in color palette; we aren't using one, so 0 for defalt
	DIB_header.append_i32(0); // number of "important colors"; 0 for default

	std::ofstream file(file_name, std::ios::binary);
	if (!file)
		return BMP_error_t::FILE_OPEN_ERROR;

	file.write(header.str().c_str(), header.size());
	file.write(DIB_header.str().c_str(), DIB_header.size());

	for (int i = height - 1; i >= 0; i--) {
		for (int j = 0; j < width; j++) {
			std::string r(1, static_cast<char>(pixel_buffer[i][j].r));
			std::string g(1, static_cast<char>(pixel_buffer[i][j].g));
			std::string b(1, static_cast<char>(pixel_buffer[i][j].b));
			file.write(b.c_str(), b.size());
			file.write(g.c_str(), g.size());
			file.write(r.c_str(), r.size());
		}

		// padding
		for (int j = width * 3; j < row_size_on_disk; j++) {
			file.write("\0", 1);
		}
	}

	file.close();

	return BMP_error_t::IMAGE_WRITE_SUCCESS;
}


BMP_error_t Pixels::read(const std::string& file_name) {
	std::ifstream file(file_name, std::ios::binary);
	if (!file)
		return BMP_error_t::FILE_OPEN_ERROR;

	LEByteBuffer header;
	header.read_bytes(file, BMP_HEADER_SIZE);
	if (header.get_i16(0) != 0x4D42) // All BMPs must start with 0x4D42 (little endian)
		return BMP_error_t::UNSUPPORTED_IMAGE_FORMAT;

	LEByteBuffer DIB_header;
	DIB_header.read_bytes(file, DIB_HEADER_SIZE);
	if (DIB_header.get_i32(0) != DIB_HEADER_SIZE)
		return BMP_error_t::UNSUPPORTED_BMP_FORMAT;

	uint32_t width = DIB_header.get_i32(4);
	uint32_t height = DIB_header.get_i32(8);
	this->width = width;
	this->height = height;

	pixel_buffer.resize(height);
	for (auto& v : pixel_buffer) {
		v.resize(width);
	}

	uint16_t color_planes = DIB_header.get_i16(12);
	if (color_planes != 1)
		return BMP_error_t::INVALID_BMP_FORMAT;
	uint16_t bpp = DIB_header.get_i16(14);
	if (bpp != 24)
		return BMP_error_t::UNSUPPORTED_BMP_FORMAT;
	uint32_t compression = DIB_header.get_i32(16);
	if (compression != 0)
		return BMP_error_t::UNSUPPORTED_BMP_FORMAT;

	uint32_t pixel_array_offset = header.get_i32(10);
	size_t row_size_on_disk = nearest4(width * 3);
	std::unique_ptr<uint8_t[]> pixel_matrix = std::unique_ptr<uint8_t[]>(new uint8_t[row_size_on_disk * height]);

	file.seekg(pixel_array_offset, std::ios::beg);
	file.read(reinterpret_cast<char*>(pixel_matrix.get()), row_size_on_disk * height);

	for (int i = height - 1, row = 0; i >= 0; i--, row++) {
		int pixel = 0, channel = 0;
		for (int j = 0; j < 3 * width; j++) {
			switch (channel) {
			case 0: // file is in BGR format (reverse RGB)
				pixel_buffer[row][pixel].b = pixel_matrix[i * row_size_on_disk + j];
				channel = 1;
				break;
			case 1:
				pixel_buffer[row][pixel].g = pixel_matrix[i * row_size_on_disk + j];
				channel = 2;
				break;
			case 2:
				pixel_buffer[row][pixel].r = pixel_matrix[i * row_size_on_disk + j];
				channel = 0;
				pixel++;
				break;
			}
		}
	}

	file.close();

	return BMP_error_t::IMAGE_READ_SUCCESS;
}

Pixel& Pixels::pixel_at(int i, int j) {
	return pixel_buffer[i][j];
}

Pixel& Pixels::pixel_at(Position p) {
	return pixel_buffer[p.i][p.j];
}

const int blur_kernel[][Pixels::KERNEL_SIZE] = {
	 {  1,  4,  6,  4,  1 },
	 {  4, 16, 24, 16,  4 },
	 {  6, 24, 36, 24,  6 },
	 {  4, 16, 24, 16,  4 },
	 {  1,  4,  6,  4,  1 }
}; // got these values from here https://computergraphics.stackexchange.com/questions/39/how-is-gaussian-blur-implemented
#include <cmath>
void Pixels::blur() {
	Pixels new_image(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int cnt = 0;
			int sum = 0;
			int newr = 0, newg = 0, newb = 0;
			for (int i0 = 0; i0 < KERNEL_SIZE; i0++) {
				for (int j0 = 0; j0 < KERNEL_SIZE; j0++) {
					int i1 = i + i0 - KERNEL_SIZE / 2;
					int j1 = j + j0 - KERNEL_SIZE / 2;
					if (in_bounds(i1, 0, height - 1) && in_bounds(j1, 0, width - 1)) {
						cnt++;
						sum += blur_kernel[i0][j0];
						newr += blur_kernel[i0][j0] * (int)pixel_at(i1, j1).r;
						newg += blur_kernel[i0][j0] * (int)pixel_at(i1, j1).g;
						newb += blur_kernel[i0][j0] * (int)pixel_at(i1, j1).b;
					}
				}
			}
			newr = round((float)newr / (float)sum);
			newg = round((float)newg / (float)sum);
			newb = round((float)newb / (float)sum);
			new_image.pixel_at(i, j) = Pixel(newr, newg, newb);
		}
	}
	*this = new_image;
}