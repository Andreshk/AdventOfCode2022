#include <fmt/core.h>
#include <fstream>
#include <string>
#include <algorithm> // std::ranges::{sort,adjacent_find}
#include <ranges> // std::views::slide

int day6(const char* filename, const int n) {
#if 1
	// Allocates only 2*N bytes, reads file byte by byte
	std::string buff(n, '\0'); // SSO to the rescue
	std::ifstream file{ filename };
	file.read(buff.data(), n);
	for (int i = 0; file; file.get(buff[i % n]), ++i) {
		std::string tmp = buff;
		std::ranges::sort(tmp);
		if (std::ranges::adjacent_find(tmp) == tmp.end()) {
			return i + n;
		}
	}
#else
	// Heap memory goes brrr
	std::string line;
	std::ifstream{ filename } >> line; // no whitespace in input
	for (std::span<char> rng : line | std::views::slide(n)) {
		std::string tmp(rng.begin(), rng.end());
		std::ranges::sort(tmp);
		if (std::ranges::adjacent_find(tmp) == tmp.end()) {
			return rng.data() - line.data() + n;
		}
	}
#endif
	return 0;
}

int main6() {
	fmt::print("{}\n", day6("input/6test.txt", 4));  // 7
	fmt::print("{}\n", day6("input/6full.txt", 4));  // 1134
	fmt::print("{}\n", day6("input/6test.txt", 14)); // 19
	fmt::print("{}\n", day6("input/6full.txt", 14)); // 2263
	return 0;
}