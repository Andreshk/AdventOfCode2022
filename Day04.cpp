#include "Lines.h"
#include <fmt/core.h>
#include <cstdio> // scanf
#include <cassert>
#include <algorithm>

// Whether [a;b] is fully contained inside [c;d] or vice versa
bool contains(int a, int b, int c, int d) {
	return ((a <= c && b >= d) || (c <= a && d >= b));
}
// Whether [a;b] intersects with [c;d] (there may be a shorter way to check, idc)
bool overlaps(int a, int b, int c, int d) {
	return ((a <= c && c <= b) || (a <= d && d <= b) || (c <= a && a <= d) || (c <= b && b <= d));
}

using Func = bool(int, int, int, int);
int day4(const char* filename, Func f) {
	return std::ranges::count_if(lines(filename), [&](std::string_view line) {
		int a, b, c, d;
		[[maybe_unused]] const int numParsed = sscanf_s(line.data(), "%d-%d,%d-%d", &a, &b, &c, &d);
		assert(numParsed == 4);
		return f(a, b, c, d);
	});
}

int main4() {
	fmt::print("{}\n", day4("input/4test.txt", contains)); // 2
	fmt::print("{}\n", day4("input/4full.txt", contains)); // 534
	fmt::print("{}\n", day4("input/4test.txt", overlaps)); // 4
	fmt::print("{}\n", day4("input/4full.txt", overlaps)); // 841
	return 0;
}