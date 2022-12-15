#include "Lines.h"
#include "IntPairs.h"
#include <fmt/core.h>
#include <ranges>
#include <vector>
#include <cstdio>
#include <algorithm>

using Map = std::vector<std::pair<IntPair, IntPair>>;
// Parses the given file to a list of sensor-beacon position pairs
Map getMap(const char* filename) {
	Map map;
	for (std::string_view str : lines(filename)) {
		IntPair s, b;
		sscanf_s(str.data(), "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d", &s.i, &s.j, &b.i, &b.j);
		map.emplace_back(s, b);
	}
	return map;
}
// Gathers the (possibly overlapping) subintervals of the given row, covered by any sensor
std::vector<IntPair> getIntervals(const Map& map, const int row) {
	std::vector<IntPair> intervals;
	auto dist = [](IntPair x, IntPair y) { return abs(x.i - y.i) + abs(x.j - y.j); };
	for (const auto& [s, b] : map) {
		const int dx = (dist(s, b) - abs(s.j - row));
		if (dx >= 0) {
			intervals.push_back({ s.i - dx,s.i + dx });
		}
	}
	return intervals;
}

int day15(const char* filename, const int row) {
	const auto map = getMap(filename);
	std::vector<IntPair> covs = getIntervals(map, row);
	std::vector<int> excls;
	for (const auto& [s, b] : map) {
		if (s.j == row) { excls.push_back(s.i); }
		if (b.j == row) { excls.push_back(b.i); }
	}
	std::ranges::sort(excls);
	// We could try merging the intervals, but splitting them by the excluded position afterwards is annoying,
	// so just brute-force count from the left-most covered position to the right-most :)
	const int min = std::ranges::min(covs | std::views::transform(&IntPair::i));
	const int max = std::ranges::max(covs | std::views::transform(&IntPair::j));
	return std::ranges::count_if(std::views::iota(min, max + 1),
		[&](const int i) {
			return std::ranges::any_of(covs, [&](IntPair p) { return p.i <= i && i <= p.j; })
				&& !std::ranges::binary_search(excls, i);
		});
}

int64_t day15_2(const char* filename, int m) {
	const auto map = getMap(filename);
	for (int row = 0; row <= m; ++row) {
		std::vector<IntPair> intervals = getIntervals(map, row);
		// Intervals are best merged after sorting by start and iterating in order
		std::ranges::sort(intervals, std::ranges::less{}, &IntPair::i);
		IntPair merged = intervals[0];
		for (const auto& [i, j] : std::views::drop(intervals, 1)) {
			assert(i <= merged.j + 2);
			if (i == merged.j + 2) { // Found the gap
				return int64_t(i - 1) * 4'000'000 + row;
			} else if (j > merged.j) { // Expand the coverage w/ current
				merged.j = j;
			}
		}
	}
	std::terminate(); // There should always be a solution
}

int main15() {
	fmt::print("{}\n", day15("input/15test.txt", 10));          // 26
	fmt::print("{}\n", day15("input/15full.txt", 2'000'000));   // 4737443
	fmt::print("{}\n", day15_2("input/15test.txt", 20));        // 56000011
	fmt::print("{}\n", day15_2("input/15full.txt", 4'000'000)); // 11482462818989, takes ~20s in Debug
	return 0;
}
