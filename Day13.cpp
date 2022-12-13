#include "Lines.h"
#include <fmt/core.h>
#include <ranges> // std::views::lazy_split
#include <vector>
#include <algorithm> // std::ranges::{lexicographical_compare,count_if}

class Packet {
	int value = -1;
	std::vector<Packet> elems;
	// Main parsing function
	Packet parse(std::string_view& str) {
		Packet p;
		if (std::isdigit(str[0])) {
			p.value = 0;
			while (std::isdigit(str[0])) {
				p.value = 10 * p.value + (str[0] - '0');
				str.remove_prefix(1);
			}
		} else if (str[0] == ']') {
			// Empty packet is valid, nothing to do
		} else {
			assert(str[0] == '[');
			str.remove_prefix(1);
			while (true) {
				p.elems.push_back(parse(str));
				if (str[0] == ']') { break; }
				assert(str[0] == ',');
				str.remove_prefix(1);
			}
			str.remove_prefix(1);
		}
		return p;
	}
	Packet() = default; // Required for the singleElem ctor to work
	Packet(int singleElem) {
		elems.push_back({});
		elems[0].value = singleElem;
	}
public:
	explicit Packet(std::string_view str) : Packet{ parse(str) } {
		assert(str.empty()); // Parsing should consume the entire string
	}
	friend bool operator<(const Packet& lhs, const Packet& rhs) {
		if (lhs.value != -1 && rhs.value != -1) { // value < value
			return (lhs.value < rhs.value);
		} else if (rhs.value != -1) { // list < value, promote the value
			return (lhs < Packet{ rhs.value });
		} else if (lhs.value != -1) { // value < list, promote the value
			return (Packet{ lhs.value } < rhs);
		} else { // list < list
			return std::ranges::lexicographical_compare(lhs.elems, rhs.elems);
		}
	}
	// std::ranges::less requires all operators to be defined...
	friend bool operator>(const Packet& lhs, const Packet& rhs) { return (rhs < lhs); }
	friend bool operator<=(const Packet& lhs, const Packet& rhs) { return !(rhs < lhs); }
	friend bool operator>=(const Packet& lhs, const Packet& rhs) { return !(lhs < rhs); }
	friend bool operator==(const Packet&, const Packet&) = default;
};

// High-order function that returns a predicate, accepting all values less than a given one
// Useful in ranges algorithms, f.e. filter(less_than(x)), count_if(less_than(x)), etc.
auto less_than(auto x) {
	return [x = std::move(x)](auto&& y) { return (y < x); };
}

void day13(const char* filename) {
	int index = 1; // indices counted from 1
	int part1 = 0;
	std::vector<Packet> packets;
	// std::views::enumerate would be nice, but range-v3's version does not accept the generator as input
	for (auto&& chunk : std::views::lazy_split(lines(filename), "")) {
		auto it = chunk.begin();
		Packet fst{ *it }; ++it;
		Packet snd{ *it }; ++it;
		assert(it == chunk.end());
		if (fst < snd) {
			part1 += index;
		}
		packets.push_back(std::move(fst));
		packets.push_back(std::move(snd));
		++index;
	}
	auto findIndex = [&](std::string_view str) {
		return std::ranges::count_if(packets, less_than(Packet{ str })) + 1; // again, indices counted from 1
	};
	const auto part2 = findIndex("[[2]]") * (findIndex("[[6]]") + 1); // another +1 since [[2]] is also smaller than [[6]]
	fmt::print("{} {}\n", part1, part2);
}

int main13() {
	day13("input/13test.txt"); // 13 140
	day13("input/13full.txt"); // 5623 20570
	return 0;
}