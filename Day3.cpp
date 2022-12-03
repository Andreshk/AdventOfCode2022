#include "Lines.h"
#include <fmt/core.h>
#include <algorithm> // std::ranges::{sort,binary_search,partition,find_if}
#include <ranges> // std::views::chunk
#include <cassert>

int score(const char c) {
	return (std::islower(c) ? (c - 'a') : (c - 'A' + 26)) + 1;
}

int task1(const char* filename) {
	return ranges::sum(lines(filename), [](std::string& line) {
		const auto half = line.begin() + line.size()/2;
		std::ranges::sort(half, line.end());
		const char c = *std::ranges::find_if(line.begin(), half, [&](char c) { return std::ranges::binary_search(half, line.end(), c); });
		return score(c);
	});
}

int task2(const char* filename) {
	return ranges::sum(std::views::chunk(lines(filename), 3), [](auto&& chunk) {
		std::string res = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		for (std::string& line : chunk) {
			std::ranges::sort(line);
			const auto [it, _] = std::ranges::partition(res, [&](char c) { return std::ranges::binary_search(line, c); });
			res.erase(it, res.end()); // this does not reallocate :)
		}
		assert(res.size() == 1);
		return score(res[0]);
	});
}

int main3() {
	fmt::print("{}\n", task1("input/3test.txt")); // 157
	fmt::print("{}\n", task1("input/3full.txt")); // 8018
	fmt::print("{}\n", task2("input/3test.txt")); // 70
	fmt::print("{}\n", task2("input/3full.txt")); // 2518
	return 0;
}