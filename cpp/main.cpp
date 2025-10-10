#include <iostream>
#include <iomanip>
#include <string>
#include <queue>
#include <limits>
#include "bmp_lib.hpp"
#include "utils.hpp"

float MAP_DENSITY = 0.5;
int MAP_AMPLITUDE = 48;

Pixels gen_map(int map_size) {
	Pixels pixels(map_size, map_size);
	for (int i = 0; i < map_size; i++)
	{
		for (int j = 0; j < map_size; j++)
		{
			float r = (float)rand() / (float)RAND_MAX;
			if (r <= MAP_DENSITY)
			{
				pixels.pixel_at(i, j).r = (float)rand() / (float)RAND_MAX * (float)MAP_AMPLITUDE;
				pixels.pixel_at(i, j).g = 0;
				pixels.pixel_at(i, j).b = 0;
			}
			else
			{
				pixels.pixel_at(i, j).r = 0;
				pixels.pixel_at(i, j).g = 0;
				pixels.pixel_at(i, j).b = 0;
			}
		}
	}
	pixels.blur();
	return pixels;
}

float cost(Position p1, Position p2, const std::vector<std::vector<float>>& map) {
	return angle(p1, p2, map) + p1.distance_to(p2) / 10.f;
}

int main(int argc, char* argv[])
{
	std::cout << std::setprecision(2) << std::fixed;

	int map_size = 100;

	bool read_map_from_file = false;
	std::string map_file_name = "";
	int dir = 4;
	for (int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];
		if (arg == "--help" || arg == "-h")
		{
			std::cout << "Format: {command};{arguments{:default}}: {description}\n";
			std::cout << "--help, -h;: prints this help screen\n";
			std::cout << "--map, -m;{file_name}: provide a pre-generated map. The image MUST be square. If not specified, a new map is generated and saved to map.bmp\n";
			std::cout << "--mapsize, -s;{int:" << map_size << "}: sets the map size in px\n";
			std::cout << "--mapdens, -d;{float:" << MAP_DENSITY << "}: sets the map density\n";
			std::cout << "--mapamplitude, -a;{int[0-255]:" << MAP_AMPLITUDE << "}: sets the map amplitude\n";
			std::cout << "-8d;: sets the movement to 8-directional, instead of 4\n";
			std::cout << "Example: ./main --mapsize 50 -d 0.02\n";
			std::cout << "Example: ./main -m map.bmp -8d\n";
			return 0;
		}
		else if (arg == "--map" || arg == "-m")
		{
			if (i + 1 < argc)
			{
				read_map_from_file = true;
				map_file_name = argv[++i];
			}
			else
			{
				std::cout << "Map file name must be provided after " << arg << "\n";
				return 0;
			}
		}
		else if (arg == "--mapsize" || arg == "-s")
		{
			if (i + 1 < argc)
			{
				std::string num = argv[++i];
				map_size = std::stoi(num);
			}
			else
			{
				std::cout << "Map size must be provided after " << arg << "!\n";
				return 0;
			}
		}
		else if (arg == "--mapdens" || arg == "-d")
		{
			if (i + 1 < argc)
			{
				std::string num = argv[++i];
				MAP_DENSITY = std::stof(num);
			}
			else
			{

				std::cout << "Map size must be provided after " << arg << "!\n";
				return 0;
			}
		}
		else if (arg == "--mapamplitude" || arg == "-a")
		{
			if (i + 1 < argc)
			{
				std::string num = argv[++i];
				MAP_AMPLITUDE = std::stoi(num);
			}
			else
			{
				std::cout << "Map amplitude must be provided after " << arg << "!\n";
				return 0;
			}
		}
		else if (arg == "-8d")
		{
			dir = 8;
		}
		else {
			std::cout << "Unknown argument: " << arg;
			return 0;
		}
	}

	Pixels pixels;
	if (!read_map_from_file) {
		pixels = gen_map(map_size);
		BMP_error_t err = pixels.save("map.bmp");
		if (err != BMP_error_t::IMAGE_WRITE_SUCCESS) {
			std::cout << "Generated map could not be saved, continuing...\n";
		}
	}
	else {
		BMP_error_t err = pixels.read(map_file_name);
		if (err != BMP_error_t::IMAGE_READ_SUCCESS) {
			std::cout << "Image at " << map_file_name << " could not be read\n";
			return 0;
		}
	}

	// Actually solving the problem
	std::vector<std::vector<float>> map(pixels.get_height());
	for (int i = 0; i < pixels.get_height(); i++) {
		for (int j = 0; j < pixels.get_width(); j++) {
			map[i].push_back(pixels.pixel_at(i, j).r / 255.f);
		}
	}

	const Position dest = { pixels.get_height() - 1, pixels.get_width() - 1 };
	const Position init = { 0, 0 };

	const Position not_visited = { -1, -1 };
	std::vector<std::vector<Position>> visited(pixels.get_height());
	std::vector<std::vector<float>> distance(pixels.get_height());
	for (int i = 0; i < pixels.get_height(); i++)
	{
		for (int j = 0; j < pixels.get_width(); j++)
		{
			visited[i].push_back(not_visited);
			distance[i].push_back(std::numeric_limits<float>::infinity());
		}
	}

	std::priority_queue<pq_entry, std::vector<pq_entry>, std::greater<pq_entry>> pq;
	pq.push({ 0.f, {init, init} });
	while (!pq.empty()) {
		pq_entry pqe = pq.top();
		float len = pqe.first;
		Position pos = pqe.second.first;
		Position from = pqe.second.second;
		pq.pop();

		if (visited[pos.i][pos.j] != not_visited)
			continue;
		visited[pos.i][pos.j] = from;

		if (pos == dest)
			break;

		for (int i = 0; i < dir; i++) {
			Position new_pos = pos + D(dir)[i];
			if (in_bounds(new_pos.i, 0, pixels.get_height() - 1) && in_bounds(new_pos.j, 0, pixels.get_width() - 1) && ok_slope(pos, new_pos, map)) {
				float new_dist = len + cost(pos, new_pos, map);
				if (new_dist < distance[new_pos.i][new_pos.j]) {
					distance[new_pos.i][new_pos.j] = new_dist;
					pq.push({ new_dist, {new_pos, pos} });
				}
			}
		}
	}

	if (distance[dest.i][dest.j] != std::numeric_limits<float>::infinity()) {
		std::vector<Position> path;

		Position old_pos = not_visited;
		Position curr_pos = dest;
		while (curr_pos != old_pos) {
			path.push_back(curr_pos);
			old_pos = curr_pos;
			curr_pos = visited[curr_pos.i][curr_pos.j];
		}

		std::cout << "Path size: " << path.size() << "\n";
		for (int i = path.size() - 1; i >= 0; i--) {
			std::cout << path.size() - i << ": (" << path[i].i << "; " << path[i].j << ")\n";
			pixels.pixel_at(path[i]).b = 0xFF;
		}

		BMP_error_t err = pixels.save("path.bmp");
		if (err != BMP_error_t::IMAGE_WRITE_SUCCESS) {
			std::cout << "Path could not be saved to a file...\n";
		}
	}
	else {
		std::cout << "No path avaliable\n";
	}
}

