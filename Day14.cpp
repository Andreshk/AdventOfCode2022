#include "Lines.h"
#include "IntPairs.h"
#include <fmt/core.h>
#include <ranges> // std::views::{transform,as_rvalue,join}
#include <vector>
#include <cstdio> // sscanf_s
#include <algorithm> // std::ranges::{to,minmax,find_if}

// For visualization only (!)
#include <fmt/ranges.h>
#include <thread> // std::this_thread::sleep_for
#include <optional>
using namespace std::chrono_literals;
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetStdHandle,SetConsoleCursorPosition
void setCursorPosition(int row, int col) {
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	//std::cout.flush();
	SetConsoleCursorPosition(hOut, { (SHORT)col, (SHORT)row });
}

std::vector<IntPair> parseWall(const std::string& line) {
	std::vector<IntPair> res;
	std::string_view delim = " -> ";
	for (auto&& [from, _] : std::views::split(line, delim)) {
		IntPair& p = res.emplace_back();
		sscanf_s(&*from, "%d,%d", &p.i, &p.j);
	}
	return res;
}
// Standard sign function - returns -1 for negative x, 1 for positive x, 0 for zero
int sgn(int x) { return (x > 0) - (x < 0); }

void day14(const char* filename, const bool part2, std::optional<std::chrono::milliseconds> drawDelay = {}) {
	std::vector<std::vector<IntPair>> walls =
		lines(filename)
		| std::views::transform(parseWall)
		| std::views::as_rvalue
		| std::ranges::to<std::vector>();
	auto [xmin, xmax] = std::ranges::minmax(std::views::join(walls) | std::views::transform(&IntPair::i));
	auto [ymin, ymax] = std::ranges::minmax(std::views::join(walls) | std::views::transform(&IntPair::j));
	ymin = 0;
	if (part2) {
		ymax += 2;
		// Make enough horizontal space for the overflowing particles
		xmin = 500 - (ymax - ymin);
		xmax = 500 + (ymax - ymin);
		walls.push_back({ {xmin,ymax},{xmax,ymax} });
	}
	// y is vertical, x is horizontal :|
	const int n = ymax - ymin + 1;
	const int m = xmax - xmin + 1;
	const char sand = 'o';
	const char empty = ' ';
	std::vector<std::string> map(n, std::string(m, empty));
	// Helper functions
	auto inside = [&](IntPair p) { return p.i >= xmin && p.i <= xmax && p.j >= ymin && p.j <= ymax; };
	auto at = [&](IntPair p) -> char& { return map[p.j - ymin][p.i - xmin]; };
	auto draw = [&](IntPair p, char c) {
		setCursorPosition(p.j - ymin, p.i - xmin);
		fmt::print("{}", c);
	};
	// Draw the walls
	for (auto& wall : walls) {
		for (int i = 1; i < wall.size(); ++i) { // std::views::adjacent<2>
			IntPair from = wall[i - 1], to = wall[i];
			const IntPair step{ sgn(to.i - from.i), sgn(to.j - from.j) };
			while (from != to) {
				at(from) = '#';
				from += step;
			}
			at(to) = '#';
		}
	}
	if (drawDelay) { fmt::print("{}\n", fmt::join(map, "\n")); }
	// Start pouring sand
	// This is std::ranges::find_if(std::views::iota(0), [&](int) { ... }) :)
	int count = 0;
	while (true) {
		IntPair curr{ 500,0 };
		if (at(curr) == sand) { // No more space for sand
			break;
		}
		bool ok = false;
		while (inside(curr)) {
			if (drawDelay) { // Draw current position
				draw(curr, sand);
				std::this_thread::sleep_for(*drawDelay);
				draw(curr, empty);
			}
			const IntPair offs[] = { {0,1},{-1,1},{1,1} };
			// Note: there may be some unneeded comparisons in the inside() calls here
			const auto it = std::ranges::find_if(offs, [&](IntPair p) { return !inside(curr + p) || at(curr + p) == empty; });
			if (it != std::ranges::end(offs)) {
				curr += *it;
			} else { // Nowhere to go, this is the final position
				if (drawDelay) { draw(curr, sand); } // Draw final position
				at(curr) = sand;
				ok = true;
				break;
			}
		}
		if (ok) { ++count; } else { break; }
	}
	if (drawDelay) { setCursorPosition(n, 0); } // Reset cursor
	fmt::print("{}\n", count);
}

int main14() {
	//day14("input/14test.txt", false); // 24
	//day14("input/14full.txt", false); // 1078
	day14("input/14test.txt", true, 30ms); // 93
	day14("input/14full.txt", true); // 30157
	return 0;
}
