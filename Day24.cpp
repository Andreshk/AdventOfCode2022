#include "Lines.h"
#include "IntPairs.h"
#include <fmt/core.h>
#include <ranges> // std::ranges::{begin.end}
#include <vector>
#include <algorithm> // std::ranges::find
#include <queue> // std::priority_queue
#include <bit> // std::{popcount,countr_zero}, for pretty-printing only
#include <set>

// Directions in order: R D L U
const char syms[] = ">v<^#";
const IntPair offs[4] = { {0,1},{1,0},{0,-1},{-1,0} };

using Map = std::vector<std::vector<uint8_t>>;
Map parseFile(const char* filename) {
	Map m;
	for (std::string_view str : lines(filename)) {
		auto& row = m.emplace_back(str.size(), uint8_t(0));
		for (int i = 0; i < str.size(); ++i) {
			const char* bit = std::ranges::find(syms, str[i]);
			if (bit != std::ranges::end(syms)) {
				row[i] = (1 << (bit - std::ranges::begin(syms)));
			}
		}
	}
	return m;
}

void print(const Map& map) {
	for (const auto& row : map) {
		for (const uint8_t c : row) {
			if ((c & 15) == 0) {
				fmt::print("{}", (c == 0 ? '.' : '#'));
			} else {
				const int bits = std::popcount(c);
				fmt::print("{}", (bits == 1 ? syms[std::countr_zero(c)] : char('0' + bits)));
			}
		}
		fmt::print("\n");
	}
	fmt::print("\n");
}

using Point = std::pair<IntPair, int>;
int runSim(std::vector<Map>& maps, const Point& start, const IntPair goal) {
	const int n = int(maps[0].size());
	const int m = int(maps[0][0].size());
	// A* evaluation function: distance from start (in # of steps, so time) + Manhattan distance heuristic (which is admissible)
	// Note that the time can be absolute, not necessarily relative to the starting time :)
	auto eval = [&](const Point& p) { return p.second + std::abs(p.first.i - goal.i) + std::abs(p.first.j - goal.j); };
	auto cmp = [&](const Point& p1, const Point& p2) { return (eval(p1) > eval(p2)); };
	std::priority_queue<Point, std::vector<Point>, decltype(cmp)> pq{ cmp };
	pq.push(start);
	std::set<Point> visited{ start };
	while (!pq.empty()) {
		const auto [pos, time] = pq.top(); pq.pop();
		assert(visited.contains({ pos,time }));
		if (pos == goal) {
			return time;
		}
		// Make sure the simulation is run until the next timepoint
		while (time + 1 >= maps.size()) {
			const Map& prev = maps.back();
			Map next(n, std::vector<uint8_t>(m, 0));
			// Outlines stay the same
			next.front() = prev.front();
			next.back() = prev.back();
			for (int row = 1; row < n - 1; ++row) {
				next[row].front() = (1 << 4);
				next[row].back() = (1 << 4);
			}
			// Move the snowflakes
			for (int row = 1; row < n - 1; ++row) {
				for (int col = 1; col < m - 1; ++col) {
					assert(prev[row][col] <= 15);
					// Loop over the set bits only
					for (uint8_t c = prev[row][col]; c != 0; c &= (c - 1)) {
						const int i = std::countr_zero(c);
						const auto [dx, dy] = offs[i];
						if (!(prev[row + dx][col + dy] & (1 << 4))) {
							next[row + dx][col + dy] |= (1 << i);
						} else switch (i) { // Wrap back; there must be something w/ row%(n-2) and col%(m-2) here
							case 0: { next[row][1] |= (1 << i); } break;
							case 1: { next[1][col] |= (1 << i); } break;
							case 2: { next[row][m - 2] |= (1 << i); } break;
							case 3: { next[n - 2][col] |= (1 << i); } break;
						}
					}
				}
			}
			maps.push_back(std::move(next));
		}
		const Map& next = maps[time + 1];
		if (next[pos.i][pos.j] == 0 && !visited.contains({ pos, time + 1 })) {
			pq.emplace(pos, time + 1);
			visited.insert({ pos,time + 1 });
		}
		for (const IntPair off : offs) {
			// Worry about going out-of-bounds from the start & end only
			if (pos.i + off.i >= 0 && pos.i + off.i < n && next[pos.i + off.i][pos.j + off.j] == 0 && !visited.contains({ pos + off, time + 1 })) {
				pq.emplace(pos + off, time + 1);
				visited.insert({ pos + off,time + 1 });
			}
		}
	}
	std::terminate(); // Goal unreachable - shouldn't be possible
}

void day24(const char* filename) {
	std::vector<Map> maps{ parseFile(filename) };
	const int n = int(maps[0].size());
	const int m = int(maps[0][0].size());
	const IntPair start{ 0,1 };
	const IntPair goal{ n - 1,m - 2 };
	const int part1 = runSim(maps, { start,0 }, goal);
	const int tmp = runSim(maps, { goal,part1 }, start);
	const int part2 = runSim(maps, { start,tmp }, goal);
	fmt::print("{} {}\n", part1, part2);
}

int main24() {
	day24("input/24test.txt"); // 18 54
	day24("input/24full.txt"); // 343 960
	return 0;
}
