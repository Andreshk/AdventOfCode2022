#include "Lines.h"
#include <fmt/core.h>
#include <charconv>
#include <map>
#include <ranges> // std::views::{filter,values}
#include <algorithm> // std::ranges::min
#include <cassert>

// Given a folder name, generates a range of all parent folders,
// including the root (designated by the empty string (!))
cppcoro::generator<std::string> parents(std::string str) {
	while (!str.empty()) {
		co_yield str;
		str.erase(str.find_last_of('/'), -1);
	}
	co_yield str; // root, i.e. empty
}

void day7(const char* filename, const bool print) {
	// Poor man's std::filesystem::path - if only it wasn't hardcoded to use wide strings (!)
	std::string currDir = "";
	int currSize = 0;
	std::map<std::string, int> dirs; // Total sizes for each folder
	for (std::string_view line : lines(filename)) {
		int fileSize = 0;
		const auto [ptr, ec] = std::from_chars(line.data(), line.data() + line.size(), fileSize);
		if (ptr != line.data()) { // Successfully parsed a number
			currSize += fileSize;
		} else {
			// Save the last folder's size before moving to another
			if (currSize != 0) { // Do not repeat work on consecutive commands
				for (const auto& parent : parents(currDir)) {
					dirs[parent] += currSize;
				}
				currSize = 0;
			}
			if (line == "$ cd ..") {
				currDir.erase(currDir.find_last_of('/'), -1);
			} else if (line.starts_with("$ cd ") && line[5] != '/') {
				currDir += '/';
				currDir += line.substr(5);
			}
		}
	}
	// Do not forget the last folder
	for (const auto& parent : parents(currDir)) {
		dirs[parent] += currSize;
	}
	// We need to loop over the sizes only, directory structure is no longer relevant
	auto values = std::views::values(dirs); // Obtaining a view is cheap :)
	const int part1 = ranges::sum(std::views::filter(values, [](int x) { return (x < 100'000); }));
	const int totalSize = dirs[""];
	assert(totalSize != 0 && totalSize <= 70'000'000);
	const int part2 = std::ranges::min(std::views::filter(values, [&](int x) { return (totalSize - x <= 40'000'000); }));
	fmt::print("{} {}\n", part1, part2);
	if (print) {
		fmt::print("total = {}\n", totalSize);
		for (const auto& [dir, size] : dirs) {
			fmt::print("{} -> {}\n", dir, size);
		}
		fmt::print("\n");
	}
}

int main7() {
	day7("input/7test.txt", true);  // 95437 24933642
	day7("input/7full.txt", false); // 1743217 8319096
	return 0;
}