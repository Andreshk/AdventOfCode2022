#include "Lines.h"
#include "IntPairs.h"
#include <fmt/core.h>
#include <ranges> // std::views::{transform,filter,iota}
#include <set>
#include <map>
#include <array>
#include <algorithm> // std::ranges::{minmax,all_of,any_of}

enum Dir { N, S, W, E };
// For a given direction, offsets to the neighbouring points an elf has to check before moving in that direction
std::array<IntPair, 3> fromDir(const Dir d) {
	switch (d) {
	case N: { return { IntPair{-1,-1},{-1,0},{-1,1} }; }
	case S: { return { IntPair{1,-1},{1,0},{1,1} }; }
	case W: { return { IntPair{-1,-1},{0,-1},{1,-1} }; }
	case E: { return { IntPair{-1,1},{0,1},{1,1} }; }
	default: std::terminate();
	}
}

void day23(const char* filename, const int numRounds = 10, const bool shouldPrint = false) {
	std::set<IntPair> elves;
	// for (auto&& [i, line] : std::views::enumerate(...))
	for (int i = 0; std::string_view line : lines(filename)) {
		for (int j = 0; j < line.size(); ++j) {
			if (line[j] == '#') {
				elves.insert({ i,j });
			}
		}
		++i;
	}
	auto print = [&]() {
		const auto [xmin, xmax] = std::ranges::minmax(elves | std::views::transform(&IntPair::i));
		const auto [ymin, ymax] = std::ranges::minmax(elves | std::views::transform(&IntPair::j));
		for (int i = xmin - 1; i <= xmax + 1; ++i) {
			for (int j = ymin - 1; j <= ymax + 1; ++j) {
				fmt::print("{}", (elves.contains({ i,j }) ? '#' : '.'));
			}
			fmt::print("\n");
		}
		fmt::print("\n");
	};
	constexpr IntPair offs[8] = { {-1,0},{-1,1},{1,0},{1,-1},{0,-1},{-1,-1},{0,1},{1,1} };
	static_assert(offs[2 * N] == IntPair{ -1,0 } && offs[2 * S] == IntPair{ 1,0 }
			   && offs[2 * W] == IntPair{ 0,-1 } && offs[2 * E] == IntPair{ 0,1 });
	if (shouldPrint) { print(); }
	int round = 0;
	while (true) {
		std::map<IntPair, Dir> proposed; // each elf's direction for moving
		std::map<IntPair, int> moves; // # of elves intending to move to a position
		for (const IntPair e : elves | std::views::filter([&](IntPair e) { return std::ranges::any_of(offs, [&](IntPair off) { return elves.contains(e + off); }); })) {
			for (const Dir d : std::views::iota(round, round + 4) | std::views::transform([](int k) { return Dir(k%4); })) {
				if (std::ranges::all_of(fromDir(d), [&](IntPair off) { return !elves.contains(e + off); })) {
					proposed[e] = d;
					++moves[e + offs[2*d]];
					break;
				}
			}
		}
		// Nothing more to do
		if (proposed.empty()) {
			fmt::print("numRounds: {}\n", round + 1);
			break;
		}
		std::set<IntPair> newElves;
		for (const IntPair e : elves) {
			if (const auto it = proposed.find(e); it == proposed.end()) { // Elf does not move
				const auto [_, inserted] = newElves.insert(e);
				assert(inserted);
			} else {
				const Dir d = it->second;
				const bool canMove = (moves.at(e + offs[2 * d]) == 1);
				const auto [_, inserted] = newElves.insert(canMove ? e + offs[2 * d] : e);
				assert(inserted);
			}
		}
		assert(newElves.size() == elves.size());
		elves = newElves;
		if (shouldPrint) { print(); }
		if (round++ == numRounds) {
			const auto [xmin, xmax] = std::ranges::minmax(elves | std::views::transform(&IntPair::i));
			const auto [ymin, ymax] = std::ranges::minmax(elves | std::views::transform(&IntPair::j));
			fmt::print("Round 10: {}\n", (xmax - xmin + 1) * (ymax - ymin + 1) - elves.size());
		}
	}
}

int main23() {
	day23("input/23test.txt"); // 110 20
	day23("input/23full.txt"); // 3931 944
	return 0;
}
