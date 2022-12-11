#include "Lines.h"
#include <fmt/core.h>
#include <ranges> // std::views::split
#include <vector>
#include <iterator> // std::istreambuf_iterator
#include <algorithm> // std::ranges::partial_sort
#include <cassert>

struct Monkey {
	std::vector<int> items;
	char op;
	int arg;
	int test;
	int idxs[2];
	int numInsp = 0;
};

std::vector<Monkey> parse2(const char* filename) {
	std::vector<Monkey> monkeys;
	std::ifstream file{ filename };
	const std::string str{ std::istreambuf_iterator<char>(file), {} }; // May not be the fastest, idc
	const std::string_view delim = "\n\n";
	for (auto&& chunk : std::views::split(str, delim)) {
		Monkey& curr = monkeys.emplace_back();
		const char* from = chunk.data() + strlen("Monkey ?:\n  Starting items: ");
		const char* to = strstr(from, "\n  O");
		int numParsed = -1;
		while (from < to) {
			numParsed = sscanf_s(from, "%d", &curr.items.emplace_back());
			assert(numParsed == 1);
			from = strchr(from, ' ') + 1; // Find the next space & skip it to prepare for the next number
		}
		char arg[4] = { '\0' }; // Whether a two-digit number, or the string "old" => make way for the '\0'
		numParsed = sscanf_s(from, " Operation: new = old %c %s\n  Test: divisible by %d\n  If true: throw to monkey %d\n  If false: throw to monkey %d",
			&curr.op, 1, &arg[0], 4, &curr.test, &curr.idxs[true], &curr.idxs[false]);
		assert(numParsed == 5);
		if (sscanf_s(arg, "%d", &curr.arg) == 0) {
			curr.arg = -1;
		}
	}
	return monkeys;
}

int64_t day11(const char* filename, const int numRounds, const int div) {
	std::vector<Monkey> monkeys = parse2(filename);
	const int mult = ranges::product(monkeys, &Monkey::test);
	for (int round = 0; round < numRounds; ++round) {
		for (Monkey& m : monkeys) {
			for (const int64_t old : m.items) {
				const int64_t arg = (m.arg == -1 ? old : m.arg);
				const int worry = int(((m.op == '+' ? old + arg : old * arg) / div) % mult); // Downcasting after % mult is ok
				const int idx = m.idxs[(worry % m.test == 0)];
				assert(&monkeys[idx] != &m); // Do not reinsert into a vector while traversing it (!)
				monkeys[idx].items.push_back(worry);
				++m.numInsp;
			}
			m.items.clear();
		}
	}
	std::ranges::partial_sort(monkeys, monkeys.begin() + 2, std::greater{}, &Monkey::numInsp);
	return int64_t(monkeys[0].numInsp) * monkeys[1].numInsp; // ranges::product(monkeys | std::views::take(2), &Monkey::numInsp) :)
}

int main11() {
	fmt::print("{}\n", day11("input/11test.txt", 20, 3)); // 10605
	fmt::print("{}\n", day11("input/11full.txt", 20, 3)); // 56120
	fmt::print("{}\n", day11("input/11test.txt", 10000, 1)); // 2713310158
	fmt::print("{}\n", day11("input/11full.txt", 10000, 1)); // 24389045529
	return 0;
}
