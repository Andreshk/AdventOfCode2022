#include "Lines.h"
#include "IntPairs.h"
#include <fmt/core.h>
#include <ranges>
#include <vector>
#include <array>
#include <queue>
#include <algorithm>

using Coord = std::array<int, 3>;
Coord parse(std::string_view str) {
	Coord res{};
	const int numParsed = sscanf_s(str.data(), "%d,%d,%d", &res[0], &res[1], &res[2]);
	assert(numParsed == 3);
	return res;
}

void day18(const char* filename) {
	const std::vector<Coord> drops{ std::from_range, std::views::transform(lines(filename), parse) };
	/*const int n = int(drops.size());
	const int part1 = 6 * n - std::ranges::count_if(IntPairs{ n,n },
		[&](IntPair p) { return (std::abs(drops[p.i][0] - drops[p.j][0]) + std::abs(drops[p.i][1] - drops[p.j][1]) + std::abs(drops[p.i][2] - drops[p.j][2]) == 1); });*/
	// All coords are in [0..xmax)x[0..ymax)x[0..zmax)
	const int xmax = 1 + std::ranges::max(drops | std::views::transform([](Coord c) { return c[0]; }));
	const int ymax = 1 + std::ranges::max(drops | std::views::transform([](Coord c) { return c[1]; }));
	const int zmax = 1 + std::ranges::max(drops | std::views::transform([](Coord c) { return c[2]; }));
	// Pad the droplet with air on all 6 sides
	enum class State : int8_t { Empty, Lava, Air };
	std::vector<State> arr((xmax + 2) * (ymax + 2) * (zmax + 2), State::Empty);
	auto inside = [&](int x, int y, int z) { return (x >= -1 && x <= xmax && y >= -1 && y <= ymax && z >= -1 && z <= zmax); };
	auto at = [&](int x, int y, int z) -> State& {
		assert(inside(x, y, z));
		return arr[((x + 1) * (ymax + 2) + (y + 1)) * (zmax + 2) + (z + 1)];
	};
	// Perform some action for each in-bounds neighbouring cell of the given
	auto forN = [&](int x, int y, int z, auto f) {
		const int offs[6][3] = { {-1,0,0},{1,0,0},{0,-1,0},{0,1,0},{0,0,-1},{0,0,1} };
		for (const auto [dx, dy, dz] : offs) {
			if (inside(x + dx, y + dy, z + dz)) {
				f(x + dx, y + dy, z + dz);
			}
		}
	};
	for (auto [x, y, z] : drops) {
		at(x, y, z) = State::Lava;
	}
	// Run BFS to find our where the air touches
	std::queue<Coord> q;
	q.push({ -1,-1,-1 });
	at(-1, -1, -1) = State::Air;
	while (!q.empty()) {
		const auto [x, y, z] = q.front(); q.pop();
		assert(at(x, y, z) == State::Air); // Position has been marked on insertion
		forN(x, y, z, [&](int x0, int y0, int z0) {
			State& c = at(x0, y0, z0);
			if (c == State::Empty) {
				q.push({ x0,y0,z0 });
				c = State::Air;
			}
		});
	}
	//const int trapped = std::ranges::count(arr, State::Empty);
	int part1 = 0, part2 = 0;
	for (int x = -1; x <= xmax; ++x) {
		for (int y = -1; y <= ymax; ++y) {
			for (int z = -1; z <= zmax; ++z) {
				if (at(x, y, z) == State::Lava) {
					forN(x, y, z, [&](int x0, int y0, int z0) {
						part1 += (at(x0, y0, z0) != State::Lava);
						part2 += (at(x0, y0, z0) == State::Air);
					});
				}
			}
		}
	}
	fmt::print("{} {}\n", part1, part2);
}

int main18() {
	day18("input/18test.txt"); // 64 58
	day18("input/18full.txt"); // 3522 2074
	return 0;
}
