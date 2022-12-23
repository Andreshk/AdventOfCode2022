#include "Lines.h"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <charconv>
#include <array>

void day10(const char* filename) {
	int x = 1; // Register value
	int cycle = 0; // Indexing cycles from 0 is easier
	int total = 0;
	std::array<std::array<char, 40>, 6> display{};
	for (auto& row : display) { row.fill('.'); }
	// Everything the CPU performs during a cycle
	auto tick = [&]() {
		const int row = cycle / 40;
		const int pos = cycle % 40;
		if (std::abs(x - pos) <= 1) {
			display[row][pos] = '#';
		}
		++cycle;
		if ((cycle + 20) % 40 == 0) {
			total += (cycle * x);
		}
	};
	for (std::string_view line : lines(filename)) {
		tick(); // All instructions take at least one cycle
		if (line.starts_with("addx ")) {
			tick(); // Addition is complete at end of second cycle (!)
			int value = 0;
			std::from_chars(line.data() + 5, line.data() + line.size(), value);
			x += value;
		}
	}
	fmt::print("{}\n{}\n", total, fmt::join(display, "\n"));
}

int main10() {
	day10("input/10test.txt"); // 13140
	day10("input/10full.txt"); // 15020 EFUGLPAP
	return 0;
}
