#include "Lines.h"
#include <fmt/core.h>
#include <vector>
#include <algorithm>
#include <cstdio> // sscanf_s

void day5(const char* filename, const bool reverse) {
	std::vector<std::string> stacks;
	bool fill = true;
	for (std::string_view line : lines(filename)) {
		if (line.empty()) { // Switch to the next phase
			std::ranges::for_each(stacks, std::ref(std::ranges::reverse));
			fill = false;
		} else if (fill) {
			const int n = int(line.length()) / 4 + 1;
			stacks.resize(n, ""); // no-op after first iteration
			for (int i = 0; i < n; ++i) {
				const char c = line[4 * i + 1];
				if (c != ' ') {
					stacks[i].push_back(c);
				}
			}
		} else {
			int n, from, to;
			sscanf_s(line.data(), "move %d from %d to %d", &n, &from, &to);
			auto& sFrom = stacks[from - 1];
			auto& sTo = stacks[to - 1];
			if (reverse) {
				std::ranges::reverse_copy(sFrom.end() - n, sFrom.end(), std::back_inserter(sTo));
			} else {
				std::ranges::copy(sFrom.end() - n, sFrom.end(), std::back_inserter(sTo));
			}
			sFrom.erase(sFrom.end() - n, sFrom.end());
		}
	}
	for (const std::string& s : stacks) {
		fmt::print("{}", s.back());
	}
	fmt::print("\n");
}

int main5() {
	day5("input/5test.txt", true);  // CMZ
	day5("input/5full.txt", true);  // ZRLJGSCTR
	day5("input/5test.txt", false); // MCD
	day5("input/5full.txt", false); // PRTTGRFPB
	return 0;
}