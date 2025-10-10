#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include <vector>

struct Position {
	int i, j;

	bool operator==(const Position&) const;
	bool operator!=(const Position&) const;
	Position operator+(const Position&) const;
	float distance_to(const Position&) const;
	bool operator>(const Position&) const;
	bool operator<(const Position&) const;
};
using pq_entry = std::pair<float, std::pair<Position, Position>>;
// Starting right, then clockwise
const Position D4[] = {
	{0, 1},
	{1, 0},
	{0, -1},
	{-1, 0},
};
const Position D8[] = {
	{0, 1},
	{1, 1},
	{1, 0},
	{1, -1},
	{0, -1},
	{-1, -1},
	{-1, 0},
	{-1, 1},
};
constexpr Position* D(int dir) {
	if (dir != 4 && dir != 8) throw std::exception();
	return (Position*)(dir == 4 ? D4 : D8);
}

class LEByteBuffer {
private:
	std::vector<uint8_t> buffer;
public:
	LEByteBuffer() = default;

	void append_i8(uint8_t);
	void set_i8(size_t, uint8_t);
	uint8_t get_i8(size_t);
	void read_i8(std::ifstream&);

	void append_i16(uint16_t);
	void set_i16(size_t, uint16_t);
	uint16_t get_i16(size_t);
	void read_i16(std::ifstream&);

	void append_i32(uint32_t);
	void set_i32(size_t, uint32_t);
	uint32_t get_i32(size_t);
	void read_i32(std::ifstream&);

	void read_bytes(std::ifstream&, size_t);

	std::string str();
	size_t size();
};

bool in_bounds(int, int, int);

constexpr float length(float);
constexpr float height(float);
float angle(const Position&, const Position&, const std::vector<std::vector<float>>&);

#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // !M_PI

const float MAX_ANGLE = M_PI / 6.f;
bool ok_slope(const Position&, const Position&, const std::vector<std::vector<float>>&);

#endif // UTILS_HPP_INCLUDED
