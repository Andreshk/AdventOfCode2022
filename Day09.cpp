#include "Lines.h"
#include <fmt/core.h>
#include <set>
#include <charconv>
#include <algorithm> // std::clamp
#include <cmath> // std::abs
#include <cassert>

struct Coord {
	int x, y;
	auto operator<=>(const Coord&) const = default;
};
// Pulls tail with 1 position, given that head was previously pulled with 1 position
void pull(const Coord& head, Coord& tail) {
	constexpr int offs[5][5] = {
		{-1,-1,-1,-1,-1},
		{-1, 0, 0, 0,-1},
		{ 0, 0, 0, 0, 0},
		{ 1, 0, 0, 0, 1},
		{ 1, 1, 1, 1, 1} };
	const int dx = (head.x - tail.x);
	const int dy = (head.y - tail.y);
	tail.x += offs[dx+2][dy+2];
	tail.y += offs[dy+2][dx+2];
	// The new dx,dy
	assert(std::abs(head.x - tail.x) <= 1 && std::abs(head.y - tail.y) <= 1);
}

int day9(const char* filename, const int k) {
	std::vector<Coord> knots(k, { 0,0 });
	std::set<Coord> positions{ knots.back() };
	for (std::string_view line : lines(filename)) {
		const char dir = line[0];
		int steps = 0;
		std::from_chars(line.data() + 2, line.data() + line.size(), steps);
		for (int i = 0; i < steps; ++i) {
			Coord& head = knots.front();
			if (dir == 'U') { ++head.x; }
			else if (dir == 'D') { --head.x; }
			else if (dir == 'L') { --head.y; }
			else { ++head.y; }

			for (int j = 1; j < k; ++j) {
				pull(knots[j - 1], knots[j]);
			}
			positions.insert(knots.back());
		}
	}
	return int(positions.size());
}

int main9() {
	fmt::print("{}\n", day9("input/9test.txt", 2)); // 13
	fmt::print("{}\n", day9("input/9full.txt", 2)); // 6284
	fmt::print("{}\n", day9("input/9test.txt", 10)); // 1
	fmt::print("{}\n", day9("input/9full.txt", 10)); // 2661
	return 0;
}
