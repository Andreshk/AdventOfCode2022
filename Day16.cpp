#include "Lines.h"
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <cstdio> // sscanf_s
#include <ranges>
#include <chrono> // for logging purposes
#include <vector>
#include <map>
#include <algorithm>

auto parse(const char* filename) {
	// Two-symbol vertex labels are encoded in a single int16_t
	std::map<int16_t, int> rates;
	std::map<int16_t, std::map<int16_t, int>> dists;
	for (std::string_view line : lines(filename)) {
		const char* from = line.data();
		const char* to = line.data() + line.size();
		char v[3] = { '\0' };
		int x = 0;
		sscanf_s(from, "Valve %s has flow rate=%d", v, 3, &x);
		const int label = (v[0] * 256 + v[1]);
		rates[label] = x;
		from += (line.find("valve") + 6);
		from += (from[0] == ' ');
		while (from < to) {
			dists[label][from[0] * 256 + from[1]] = 1;
			from += 4;
		}
	}
	// Make the graph complete
	const int inf = int(2 * rates.size()); // There can be no paths of this distance
	auto vertices = rates | std::views::keys;
	for (const int16_t u : vertices) {
		dists[u][u] = 0;
		for (const int16_t v : vertices) {
			dists[u].emplace(v, inf);
		}
	}
	// Floyd-Warshall
	for (const int16_t k : vertices) {
		for (const int16_t i : vertices) {
			for (const int16_t j : vertices) {
				if (dists[i][j] > dists[i][k] + dists[k][j]) {
					dists[i][j] = dists[i][k] + dists[k][j];
				}
			}
		}
	}
	// Remap the active valves to [0..n] and remove the rest
	const int16_t start = ('A' * 256 + 'A');
	rates[start] = 42'000; // temporarily map the starting valve as active
	const std::vector<int16_t> actives = rates
		| std::views::filter([](const auto& p) { return (p.second != 0); })
		| std::views::keys
		| std::ranges::to<std::vector>();
	assert(actives[0] == start);
	const int n = int(actives.size());
	assert(n < 32); // Use uint64_t as bitset otherwise, but good luck w/ part2
	std::vector<int> newRates(n, 0);
	std::vector<std::vector<int>> newDists(n, std::vector<int>(n, 0));
	for (int u = 0; u < n; ++u) {
		const int16_t key = actives[u];
		newRates[u] = rates[key];
		for (const auto& [key2, d] : dists[key]) {
			if (rates[key2] > 0) {
				const int v = int(std::ranges::find(actives, key2) - actives.begin());
				newDists[u][v] = d;
			}
		}
	}
	newRates[0] = 0; // revert AA to inactive
	return std::make_pair(std::move(newRates), std::move(newDists));
}

struct Log {
	int calls = 0;
	int hits = 0;
};
using Point = std::tuple<int, uint32_t, int>;
int solve(const std::vector<int>& rates, const std::vector<std::vector<int>>& dists, std::map<Point, int>& M, Log& log, Point p) {
	const auto& [curr, visited, rem] = p;
	auto& [calls, hits] = log;
	assert(rem > 0 && (visited & (1 << curr))); // Make sure not to revisit curr in the recursive calls
	++calls;
	if (const auto it = M.find(p); it != M.end()) {
		++hits;
		return it->second;
	}
	// This is ranges::max(iota(...) | filter(...) | transform(...)), but max does not accept empty ranges
	int res = 0;
	for (int u = 0; u < rates.size(); ++u) {
		const int time = dists[curr][u] + 1;
		if (!(visited & (1 << u)) && rem > time) { // No unnecessary visits
			res = std::max(res, (rem - time) * rates[u] + solve(rates, dists, M, log, Point{ u, visited | (1 << u), rem - time }));
		}
	}
	M[p] = res;
	return res;
}

template <typename T>
void day16(const char* filename) {
	const auto [rates, dists] = parse(filename);
	// Part1: find the best flow for one position
	std::map<Point, int> M;
	Log log{};
	auto start = std::chrono::system_clock::now();
	const int part1 = solve(rates, dists, M, log, Point{ 0, (1 << 0), 30 });
	auto time = std::chrono::duration_cast<std::chrono::duration<long long, T>>(std::chrono::system_clock::now() - start);
	fmt::print("{} ({}, {} calls, {} hits, {} points)\n", part1, time, log.calls, log.hits, M.size());
	// Part2: try every possible disjoint pair of subsets for the player & the elephant to visit
	log = {};
	start = std::chrono::system_clock::now();
	const int part2 = std::ranges::max(std::views::transform(std::views::iota(0, (1 << (rates.size() - 1))),
		[&](const uint32_t visited) {
			std::map<Point, int> M1, M2;
			return solve(rates, dists, M1, log, Point{ 0, (visited << 1) | 1, 26 })
				+ solve(rates, dists, M2, log, Point{ 0, ((~visited) << 1) | 1, 26 });
		}));
	time = std::chrono::duration_cast<std::chrono::duration<long long, T>>(std::chrono::system_clock::now() - start);
	fmt::print("{} ({}, {} calls, {} hits)\n", part2, time, log.calls, log.hits);
}

int main16() {
	day16<std::micro>("input/16test.txt"); // 1651 1707
	day16<std::milli>("input/16full.txt"); // 1754 2474
	return 0;
}
