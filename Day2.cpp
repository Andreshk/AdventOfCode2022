#include "Lines.h"
#include <fmt/core.h>
#include <ranges> // std::views::transform
#include <array>

// Given a functor that determines the score for a single string,
// precomputes the tables of scores for all 9 strings.
template <typename Func>
constexpr std::array<int, 9> makeArray(Func&& f) {
	std::array<int, 9> res = {};
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			const char str[] = { char('A' + i), ' ', char('X' + j), '\0'};
			res[3 * i + j] = f(str);
		}
	}
	return res;
}

constexpr auto scores1 = makeArray([](std::string_view str) {
	const int theirs = (str[0] - 'A');
	const int mine = (str[2] - 'X');
	// Remap (mine - theirs) from [-2,-1,0,1,2] to 3*[2,0,1,2,0]
	return (mine + 1) + 3 * ((mine - theirs + 4) % 3);
});

constexpr auto scores2 = makeArray([](std::string_view str) {
	const int theirs = (str[0] - 'A');
	const int res = (str[2] - 'X');
	// Remap (theirs + res) from [0,1,2,3,4] to [3,1,2,3,1]
	const int mine = (theirs + res + 2) % 3 + 1;
	return mine + (res * 3); // { 0,3,6 }[res]
});

int sumTopKRanges(const char* filename, const std::array<int, 9>& scores) {
	return ranges::sum(
		lines(filename)
		| std::views::transform([&](std::string_view line) {
			return scores[3 * (line[0] - 'A') + (line[2] - 'X')];
		})
	);
}

int main2() {
	fmt::print("{}\n", sumTopKRanges("input/2test.txt", scores1)); // 15
	fmt::print("{}\n", sumTopKRanges("input/2full.txt", scores1)); // 14297
	fmt::print("{}\n", sumTopKRanges("input/2test.txt", scores2)); // 12
	fmt::print("{}\n", sumTopKRanges("input/2full.txt", scores2)); // 10498
	return 0;
}