#include <stdexcept>
#include <sstream>
#include <fstream>
#include <memory>
#include "utils.hpp"

bool Position::operator==(const Position& other) const {
	return i == other.i && j == other.j;
}
bool Position::operator!=(const Position& other) const {
	return i != other.i || j != other.j;
}
Position Position::operator+(const Position& other) const {
	return { i + other.i, j + other.j };
}
float Position::distance_to(const Position& other) const {
	float i = fabs(this->i - other.i);
	float j = fabs(this->j - other.j);
	return sqrt(i * i + j * j);
}
bool operator>(const pq_entry& a, const pq_entry& b) {
	if (a.first != b.first)
		return a.first > b.first;
	if (a.second.first != b.second.first)
		return a.second.first > b.second.first;
	return a.second.second > b.second.second;
}
bool operator<(const pq_entry& a, const pq_entry& b) {
	if (a.first != b.first)
		return a.first < b.first;
	if (a.second.first != b.second.first)
		return a.second.first < b.second.first;
	return a.second.second < b.second.second;
}

bool Position::operator>(const Position& other) const {
	if (i != other.i)
		return i > other.i;
	return j > other.j;
}
bool Position::operator<(const Position& other) const {
	if (i != other.i)
		return i < other.i;
	return j < other.j;
}

void LEByteBuffer::append_i8(uint8_t byte) {
	buffer.push_back(byte);
}
void LEByteBuffer::set_i8(size_t byte_offset, uint8_t byte) {
	if (byte_offset < buffer.size()) {
		buffer[byte_offset] = byte;
	}
	else {
		std::stringstream ss;
		ss << "byte_offset(" << byte_offset << ") out of bounds. (buffer.size() = " << buffer.size() << ")";
		throw std::out_of_range(std::string(ss.str()));
	}
}
uint8_t LEByteBuffer::get_i8(size_t byte_offset) {
	if (byte_offset < buffer.size()) {
		return buffer[byte_offset];
	}
	else {
		std::stringstream ss;
		ss << "byte_offset(" << byte_offset << ") out of bounds. (buffer.size() = " << buffer.size() << ")";
		throw std::out_of_range(std::string(ss.str()));
	}
}
void LEByteBuffer::read_i8(std::ifstream& file) {
	uint8_t byte;
	file.read(reinterpret_cast<char*>(&byte), sizeof(byte));
	append_i8(byte);
}

void LEByteBuffer::append_i16(uint16_t word) {
	buffer.push_back(word & 0xFF);
	buffer.push_back(word >> 8);
}
void LEByteBuffer::set_i16(size_t byte_offset, uint16_t word) {
	if (byte_offset + 1 < buffer.size()) {
		buffer[byte_offset] = word & 0xFF;
		buffer[byte_offset + 1] = word >> 8;
	}
	else {
		std::stringstream ss;
		ss << "byte_offset(" << byte_offset << ") out of bounds. (buffer.size() = " << buffer.size() << ")";
		throw std::out_of_range(std::string(ss.str()));
	}
}
uint16_t LEByteBuffer::get_i16(size_t byte_offset) {
	if (byte_offset + 1 < buffer.size()) {
		uint16_t x = 0;
		x = static_cast<uint16_t>(buffer[byte_offset + 1]);
		x = (x << 8) + static_cast<uint16_t>(buffer[byte_offset]);
		return x;
	}
	else {
		std::stringstream ss;
		ss << "byte_offset(" << byte_offset << ") out of bounds. (buffer.size() = " << buffer.size() << ")";
		throw std::out_of_range(std::string(ss.str()));
	}
}
void LEByteBuffer::read_i16(std::ifstream& file) {
	uint16_t word;
	file.read(reinterpret_cast<char*>(&word), sizeof(word));
	append_i16(word);
}

void LEByteBuffer::append_i32(uint32_t dword) {
	buffer.push_back(dword & 0xFF);
	buffer.push_back((dword >> 8) & 0xFF);
	buffer.push_back((dword >> 16) & 0xFF);
	buffer.push_back(dword >> 24);
}
void LEByteBuffer::set_i32(size_t byte_offset, uint32_t dword) {
	if (byte_offset + 3 < buffer.size()) {
		buffer[byte_offset] = dword & 0xFF;
		buffer[byte_offset + 1] = (dword >> 8) & 0xFF;
		buffer[byte_offset + 2] = (dword >> 16) & 0xFF;
		buffer[byte_offset + 3] = dword >> 24;
	}
	else {
		std::stringstream ss;
		ss << "byte_offset(" << byte_offset << ") out of bounds. (buffer.size() = " << buffer.size() << ")";
		throw std::out_of_range(std::string(ss.str()));
	}
}
uint32_t LEByteBuffer::get_i32(size_t byte_offset) {
	if (byte_offset + 1 < buffer.size()) {
		uint32_t x = 0;
		x = static_cast<uint32_t>(buffer[byte_offset + 3]);
		x = (x << 8) + static_cast<uint32_t>(buffer[byte_offset + 2]);
		x = (x << 8) + static_cast<uint32_t>(buffer[byte_offset + 1]);
		x = (x << 8) + static_cast<uint32_t>(buffer[byte_offset]);
		return x;
	}
	else {
		std::stringstream ss;
		ss << "byte_offset(" << byte_offset << ") out of bounds. (buffer.size() = " << buffer.size() << ")";
		throw std::out_of_range(std::string(ss.str()));
	}
}
void LEByteBuffer::read_i32(std::ifstream& file) {
	uint32_t dword;
	file.read(reinterpret_cast<char*>(&dword), sizeof(dword));
	append_i32(dword);
}

void LEByteBuffer::read_bytes(std::ifstream& file, size_t cnt) {
	std::unique_ptr<uint8_t[]> bytes = std::unique_ptr<uint8_t[]>(new uint8_t[cnt]);
	file.read(reinterpret_cast<char*>(bytes.get()), cnt);
	for (int i = 0; i < cnt; i++) {
		buffer.push_back(bytes.get()[i]);
	}
}

std::string LEByteBuffer::str() {
	std::string s(buffer.size(), '\0');
	std::copy(buffer.begin(), buffer.end(), s.begin());
	return s;
}
size_t LEByteBuffer::size() {
	return buffer.size();
}

bool in_bounds(int p, int left, int right) {
	return left <= p && p <= right;
}

constexpr float length(float x) {
	return x * 0.1f;
}
constexpr float height(float x) {
	return x * 3.f;
}
float angle(const Position& p1, const Position& p2, const std::vector<std::vector<float>>& map) {
	float x = length(p1.distance_to(p2));
	float y = height(fabs(map[p1.i][p1.j] - map[p2.i][p2.j]));

	return atan(y / x);
}
bool ok_slope(const Position& p1, const Position& p2, const std::vector<std::vector<float>>& map) {
	return angle(p1, p2, map) <= MAX_ANGLE;
}
