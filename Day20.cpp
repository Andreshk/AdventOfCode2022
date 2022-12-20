#include "Lines.h"
#include <fmt/core.h>
#include <ranges>
#include <charconv>
#include <algorithm>

extern int sgn(int x); // Already have it in Day14.cpp, let the linker find it

int64_t day20(const char* filename, const int64_t key, const int numRuns) {
	std::vector<int64_t> xs;
	std::vector<int> idxs;
	int n = 0;
	for (std::string_view line : lines(filename)) {
		std::from_chars(line.data(), line.data() + line.size(), xs.emplace_back());
		idxs.push_back(n++);
	}
	for (auto& x : xs) { assert(INT64_MAX / key > x); x *= key; }
	for (int run = 0; run < numRuns; ++run) {
		for (int i = 0; i < n; ++i) {
			const int idx = int(std::ranges::find(idxs, i) - idxs.begin());
			assert(idx < n);
			int newIdx = (idx + xs[idx]) % (n - 1);
			if (newIdx <= 0) { newIdx += (n - 1); }
			const int dx = sgn(newIdx - idx);
			for (int i = idx; i != newIdx; i += dx) {
				std::swap(xs[i], xs[i + dx]);
				std::swap(idxs[i], idxs[i + dx]);
			}
		}
	}
	const int pos = int(std::ranges::find(xs, 0) - xs.begin());
	assert(pos != n);
	return ranges::sum(std::views::iota(1, 4), [&](int i) { return xs[pos + 1000 * i]; });
}

int main20() {
	fmt::print("{}\n", day20("input/20test.txt", 1, 1)); // 3
	fmt::print("{}\n", day20("input/20full.txt", 1, 1)); // 14888
	fmt::print("{}\n", day20("input/20test.txt", 811589153, 10)); // 1623178306
	fmt::print("{}\n", day20("input/20full.txt", 811589153, 10)); // 3760092545849
	return 0;
}
