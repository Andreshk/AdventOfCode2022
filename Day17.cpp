#include "Lines.h"
#include "IntPairs.h"
#include <fmt/core.h>
#include <ranges>
#include <vector>
#include <array>
#include <algorithm>

// Generates the rock heights after each rock i sdropped. Will run indefinitely.
cppcoro::generator<int> runSim(const char* filename) {
	std::string pattern;
	std::ifstream{ filename } >> pattern;
	const IntPair shapes[5][5] = {{ {0,0},{0,1},{0,2},{0,3},{0,0} },
								  { {1,0},{1,1},{0,1},{2,1},{1,2} },
								  { {0,0},{0,1},{0,2},{1,2},{2,2} },
								  { {0,0},{1,0},{2,0},{3,0},{0,0} },
								  { {0,0},{0,1},{1,1},{1,0},{0,0} }};
	const int shapeWidths[5] = { 4,3,3,1,2 };
	const int shapeHeights[5] = {1,3,3,4,2};
	const int shapeDepths[5] = {0,1,0,0,0};
	// to-do: do not keep the entire rock stack, only the top K rows and use them as keys to find periods
	std::vector<uint8_t> map(3, 0);
	int tick = 0;
	int height = 0; // current height of stacked rocks
	for (const int idx : std::views::iota(0)) {
		const IntPair(&shape)[5] = shapes[idx % 5];
		const int shapeHeight = shapeHeights[idx % 5];
		const int shapeWidth = shapeWidths[idx % 5];
		// to-do: this might be unnecessary if height hasn't increased in previous iteration, but whatever
		while (map.size() < height + shapeHeight + 3) { map.push_back(0); }
		IntPair pos = { height + 3, 2 };
		auto canMove = [&](int dx, int dy) {
			return std::ranges::none_of(shape, [&](IntPair p) { return (map[pos.i + p.i + dx] & (1 << (pos.j + p.j + dy))); });
		};
		while (true) {
			const int wind = (pattern[tick % pattern.size()] == '<' ? -1 : 1);
			++tick;
			if (pos.j + wind >= 0 && pos.j + wind <= 7 - shapeWidth && canMove(0, wind)) {
				pos.j += wind;
			}
			if (pos.i > 0 && canMove(-1, 0)) {
				--pos.i;
			} else {
				for (auto [i, j] : shape) {
					assert(!(map[pos.i + i] & (1 << (pos.j + j))) || (i == 0 && j == 0));
					map[pos.i + i] |= (1 << (pos.j + j));
				}
				height = std::max(height, pos.i + shapeHeight); // -1 for the floor
				break;
			}
		}
		co_yield height;
	}
}

auto findPeriod(const char* filename) {
	std::vector<int> heights{ std::from_range, std::views::take(runSim(filename), 1'000'000) };
	const int n = int(heights.size());
	for (int period = 3; period < n / 3; ++period) {
		for (int off = 0; off < period; ++off) {
			const int diff = heights[period + off] - heights[off];
			const bool ok = std::ranges::all_of(std::views::iota(2, (n - off) / period),
				[&](int k) { return heights[k * period + off] - heights[(k - 1) * period + off] == diff; });
			if (ok) {
				return std::make_tuple(period, off, std::move(heights));
			}
		}
	}
	std::terminate();
}

// to-do: while runSim() is correct, this approximation isn't (!) but it's close enough (:
void day17_final(const char* filename) {
	const auto [period, off, heights] = findPeriod(filename);
	const int diff = heights[period + off] - heights[off];
	//fmt::print("period={} after off={} diff={} baseh={}\n", period, off, diff, heights[off]);
	for (const int64_t n : { 2022ll, 1'000'000'000'000ll }) {
		const int64_t numReps = (n - off) / period;
		const int64_t rem = n - off - (period * numReps);
		//fmt::print("n={} reps={} rem={}\n", n, numReps, rem);
		fmt::print("{}\n", heights[off] + numReps * diff + heights[rem]);
	}
}

int main17() {
	day17_final("input/17test.txt"); // 3068 1514285714288
	day17_final("input/17full.txt"); // 3153 1553665689155
	return 0;
}
