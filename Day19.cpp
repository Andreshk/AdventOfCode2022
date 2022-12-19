#include "Lines.h"
#include <fmt/core.h>
#include <cstdio>
#include <vector>
#include <ranges> // std::views::{iota,transform}
#include <array>
#include <map>
#include <thread>
#include <chrono> // for debugging purposes only
#include <mutex> // for debugging purposes only
#include <algorithm> // std::ranges::all_of

struct Blueprint {
	// The required materials for each action (construct ore/clay/obsidian/geode robot or do nothing).
	std::array<std::array<int, 3>, 5> reqs = {}; // Important to zero-initialize
	// Empirical estimates for maximum reasonable amounts of resources to store - more than that shouldn't affect the result.
	// This reduces the search space for the optimal solution _extremely_
	std::array<int, 3> limits;

	static Blueprint parse(std::string_view line) {
		Blueprint b;
		const int numParsed = sscanf_s(line.data(),
			"Blueprint %*d:"
			" Each ore robot costs %d ore."
			" Each clay robot costs %d ore."
			" Each obsidian robot costs %d ore and %d clay."
			" Each geode robot costs %d ore and %d obsidian.",
			&b.reqs[0][0], &b.reqs[1][0], &b.reqs[2][0], &b.reqs[2][1], &b.reqs[3][0], &b.reqs[3][2]);
		assert(numParsed == 6);
		b.limits[0] = 2 * std::max({ b.reqs[0][0], b.reqs[1][0], b.reqs[2][0], b.reqs[3][0] });
		b.limits[1] = 2 * b.reqs[2][1];
		b.limits[2] = 2 * b.reqs[3][2];
		return b;
	}
};

// Per-component >= for arrays
bool enough(const std::array<int, 3>& xs, const std::array<int, 3>& ys) {
	return (xs[0] >= ys[0] && xs[1] >= ys[1] && xs[2] >= ys[2]); // This is all_of(iota(0,3),...)
}
// We'll calculate the maximum value for a given time, resource & robot counts
// Note that geodes are not counted as a material, since nothing depends on them - the robots just produce & dump them
struct Point {
	std::array<int, 3> resources;
	std::array<int, 4> robots;
	auto operator<=>(const Point&) const = default;
};
int solve(const Blueprint& b, std::vector<std::map<Point, int>>& M, const Point p, const int time) {
	assert(time >= 1);
	if (time == 1) { // No point in building robots at the last minute, just collect geodes
		return p.robots[3];
	} else if (time == 2) { // Try building a geode robot for an extra geode during the last minute, nothing else matters
		return 2 * p.robots[3] + enough(p.resources, b.reqs[3]);
	} else if (const auto [it, inserted] = M[time].insert(std::make_pair(p, 0)); !inserted) {
		return it->second;
	} else {
		int res = 0;
		for (int i = 0; i < 5; ++i) {
			if (enough(p.resources, b.reqs[i])) {
				Point p0 = p;
				for (int k = 0; k < 3; ++k) {
					p0.resources[k] = std::min(p0.resources[k] - b.reqs[i][k] + p0.robots[k], b.limits[k]);
				}
				// Despite the material limits, robot counts are correct
				if (i < 4) { ++p0.robots[i]; } // The last "option" is not to build any robot at all, just collect
				res = std::max(res, p.robots[3] + solve(b, M, p0, time - 1));
			}
		}
		it->second = res;
		return res;
	}
}

int day19(const char* filename, const int time, const bool part1) {
	const std::vector<Blueprint> blueprints{ std::from_range, std::views::transform(lines(filename), &Blueprint::parse) };
	std::vector<std::thread> ths;
	std::mutex fmtMutex;
	const int k = int(part1 ? blueprints.size() : std::min(3ull, blueprints.size()));
	const size_t pad = fmt::formatted_size("{}", k - 1); // for visualization only
	std::vector<int> qs(k, 0);
	for (int idx = 0; idx < k; ++idx) {
		ths.emplace_back([&](const int idx) {
			const auto start = std::chrono::system_clock::now();
			// A separate cache for each timestamp, for slightly faster operations
			std::vector<std::map<Point, int>> M(time + 1);
			const int q = solve(blueprints[idx], M, Point{ {0,0,0},{1,0,0,0} }, time);
			const auto s = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
			{
				std::lock_guard l{ fmtMutex };
				fmt::print("idx={:<{}} q={} (time={}ms, cache={})\n", idx, pad, q, s.count(), ranges::sum(M, &std::map<Point, int>::size));
			}
			qs[idx] = q;
		}, idx);
	}
	for (auto& t : ths) { t.join(); }
	if (part1) {
		return ranges::sum(std::views::iota(0, k), [&](int idx) { return (idx + 1) * qs[idx]; });
	} else {
		return ranges::product(qs);
	}
}

int main19() {
	fmt::print("{}\n", day19("input/19test.txt", 24, true)); // 33
	fmt::print("{}\n", day19("input/19full.txt", 24, true)); // 1199
	fmt::print("{}\n", day19("input/19test.txt", 32, false)); // 3472
	fmt::print("{}\n", day19("input/19full.txt", 32, false)); // 3510
	return 0;
}
