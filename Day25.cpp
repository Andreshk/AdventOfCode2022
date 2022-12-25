#include "Lines.h"
#include <fmt/core.h>
#include <array>
#include <algorithm> // std::ranges::find

const std::array<const char, 5> digits = { '=','-','0','1','2' };

int64_t decypher(std::string_view str) {
	int64_t res = 0;
	for (const char c : str) { // This is std::ranges::fold
		res = res * 5 + (std::ranges::find(digits, c) - digits.begin() - 2);
	}
	return res;
}

void day25(const char* filename) {
	int64_t sum = ranges::sum(lines(filename), decypher);
	std::string res = {};
	while (sum) { // This is Data.List.unfoldr :)
		const int64_t mod = ((sum % 5) + 2) % 5 - 2;
		res.push_back(digits[mod + 2]);
		sum -= mod;
		assert(sum % 5 == 0);
		sum /= 5;
	}
	std::ranges::reverse(res);
	fmt::print("{}\n", res);
}

int main() {
	day25("input/25test.txt"); // 2=-1=0
	day25("input/25full.txt"); // 2-121-=10=200==2==21
	return 0;
}
