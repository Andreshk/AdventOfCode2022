#include "Lines.h"
#include <fmt/core.h>
#include <vector>
#include <ranges> // lazy_split_view

// Returns the sum of the top k ranges of consecutive numbers in the file
int sumTopKRanges(const char* filename, const int k) {
	std::vector<int> xs(k, 0); // Current top k, ordered from smallest to largest
	const int delim = -1;
	for (auto&& rng : std::ranges::lazy_split_view(lines(filename, delim), delim)) {
		const int curr = ranges::sum(rng);
		assert(curr != 0);
		if (curr > xs[0]) {
			// Place at the front, then push back until necessary
			xs[0] = curr;
			for (int i = 1; i < k && xs[i - 1] > xs[i]; ++i) {
				std::swap(xs[i - 1], xs[i]);
			}
		}
	}
	return ranges::sum(xs);
}

int main1() {
	fmt::print("{}\n", sumTopKRanges("input/1test.txt", 1)); // 24000
	fmt::print("{}\n", sumTopKRanges("input/1full.txt", 1)); // 66186
	fmt::print("{}\n", sumTopKRanges("input/1test.txt", 3)); // 45000
	fmt::print("{}\n", sumTopKRanges("input/1full.txt", 3)); // 196804
	return 0;
}