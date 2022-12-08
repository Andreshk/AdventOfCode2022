#include "Lines.h"
#include <fmt/core.h>
#include <vector>
#include <ranges> // std::views::{iota,reverse}
#include <algorithm> // std::ranges::{find_if,count,all_of,max}

// Generates a range of f(i,j) for each i in [0;n) and each j in [0;m).
// Same as transform(cartesian_product(iota(0,n),iota(0,m)),
//                   [&](const auto [i,j]) { ... })) - if it were supported (:
template <typename Func>
auto genPairs(const int n, const int m, Func&& f) -> cppcoro::generator<std::decay_t<std::invoke_result_t<Func, int, int>>> {
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			co_yield f(i, j);
		}
	}
}

// Counts the number of values in r before the first that
// satisfies the predicate, but includes it in the count.
template <typename Rng, typename Pred>
int countUntil(Rng&& r, Pred&& p) {
	const auto it = std::ranges::find_if(r, p);
	return int(it - r.begin()) + (it != r.end());
}

void day8(const char* filename) {
	// C++23: containers can construct from/insert/append whole ranges
	const std::vector<std::string> map{ std::from_range, std::views::as_rvalue(lines(filename)) };	
	using std::views::iota;
	using std::views::reverse;
	const int n = int(map.size());
	const int m = int(map[0].size());
	const auto part1 = std::ranges::count(genPairs(n, m, [&](const int i, const int j) {
		const int curr = map[i][j];
		return std::ranges::all_of(iota(0, i), [&](int k) { return map[k][j] < curr; })
			|| std::ranges::all_of(iota(i + 1, n), [&](int k) { return map[k][j] < curr; })
			|| std::ranges::all_of(iota(0, j), [&](int k) { return map[i][k] < curr; })
			|| std::ranges::all_of(iota(j + 1, m), [&](int k) { return map[i][k] < curr; });
		}), true);
	const auto part2 = std::ranges::max(genPairs(n, m, [&](const int i, const int j) {
		const char curr = map[i][j];
		return countUntil(reverse(iota(0, i)), [&](int k) { return map[k][j] >= curr; })
			 * countUntil(iota(i + 1, n), [&](int k) { return map[k][j] >= curr; })
			 * countUntil(reverse(iota(0, j)), [&](int k) { return map[i][k] >= curr; })
			 * countUntil(iota(j + 1, n), [&](int k) { return map[i][k] >= curr; });
	}));
	fmt::print("{} {}\n", part1, part2);
}

int main8() {
	day8("input/8test.txt"); // 21 8
	day8("input/8full.txt"); // 1711 301392
	return 0;
}
