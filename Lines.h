#pragma once
#include <cppcoro/generator.hpp> // preferrably from github/andreasbuhr
#include <fstream>
#include <string>
#include <string_view>
#include <charconv>
#include <cassert>

// Used to iterate the lines of a file. Can be iterated as
// for (std::string_view line : lines(...)) { ... }
inline cppcoro::generator<std::string> lines(const char* filename) {
	std::string line;
	std::ifstream file{ filename };
	while (std::getline(file, line)) {
		co_yield line;
	}
}

// Used to iterate the lines of a file, containing numbers of any type, parsed by std::from_chars.
// Returns the given defaultVal for each empty line in the file.
template <typename T>
cppcoro::generator<T> lines(const char* filename, const T defaultVal) {
	for (const std::string& str : lines(filename)) {
		T val = defaultVal;
		if (!str.empty()) {
			[[maybe_unused]] const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), val);
			assert(ptr == str.data() + str.size() && ec == std::errc{});
		}
		co_yield val;
	}
}

// not std (!)
namespace ranges {
template <typename Rng, typename Proj = std::identity>
auto sum(Rng&& rng, Proj proj = {}) {
	std::decay_t<std::indirect_result_t<Proj, std::ranges::iterator_t<Rng>>> res{}; // as per rangev3's fold
	for (auto&& x : rng) { res += proj(std::forward<decltype(x)>(x)); }
	return res;
}
}
