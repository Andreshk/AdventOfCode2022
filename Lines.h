#pragma once
#include <cppcoro/generator.hpp> // preferrably from github/andreasbuhr
#include <fstream>
#include <string>
#include <string_view>
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

// not std (!)
namespace ranges {
template <typename Rng, typename Proj = std::identity>
auto sum(Rng&& rng, Proj proj = {}) {
	std::decay_t<std::invoke_result_t<Proj, std::ranges::range_reference_t<Rng>>> res{}; // as per rangev3's fold
	for (auto&& x : rng) { res += std::invoke(proj, std::forward<decltype(x)>(x)); }
	return res;
}
template <typename Rng, typename Proj = std::identity>
auto product(Rng&& rng, Proj proj = {}) {
	std::decay_t<std::invoke_result_t<Proj, std::ranges::range_reference_t<Rng>>> res{ 1 }; // as per rangev3's fold
	for (auto&& x : rng) { res *= std::invoke(proj, std::forward<decltype(x)>(x)); }
	return res;
}
}
