#include "Lines.h"
#include <fmt/core.h>
#include <ranges>
#include <variant>
#include <charconv>
#include <map>
#include <algorithm>

struct Node {
	std::string lhs, rhs; // Operand keys
	char op; // Operation
};
using Expr = std::map<std::string, std::variant<int64_t, Node>>;

struct EvalVisitor {
	const Expr& e;
	int64_t operator()(int64_t x) const { return x; }
	int64_t operator()(const Node& node) const {
		const int64_t lhs = std::visit(*this, e.at(node.lhs));
		const int64_t rhs = std::visit(*this, e.at(node.rhs));
		switch (node.op) {
		case '+': return lhs + rhs;
		case '-': return lhs - rhs;
		case '*': return lhs * rhs;
		case '/': return lhs / rhs;
		}
		assert(false); return 0;
	}
};

struct SolveVisitor {
	const Expr& e;
	int64_t operator()(int64_t x) const { return x; }
	int64_t operator()(const Node& node) const {
		if (node.lhs == "humn" || node.rhs == "humn") {
			return -1;
		} else {
			const int64_t lhs = std::visit(*this, e.at(node.lhs));
			const int64_t rhs = std::visit(*this, e.at(node.rhs));
			if (lhs == -1 || rhs == -1) { return -1; } // Propagate upwards
			switch (node.op) {
			case '+': return lhs + rhs;
			case '-': return lhs - rhs;
			case '*': return lhs * rhs;
			case '/': return lhs / rhs;
			}
			assert(false); return 0;
		}
	}
};
// Given an operation, its result and one of the operands, finds the other operand
int64_t find(char op, int64_t rhs, int64_t goal) { // Only the right operand is known
	switch (op) {
	case '+': return goal - rhs;
	case '-': return goal + rhs;
	case '*': return goal / rhs;
	case '/': return goal * rhs;
	}
	std::terminate();
}
int64_t find(int64_t lhs, char op, int64_t goal) { // Only the left operand is known
	switch (op) {
	case '+': return goal - lhs;
	case '-': return lhs - goal;
	case '*': return goal / lhs;
	case '/': return lhs / goal;
	}
	std::terminate();
}

int64_t solve(const Expr& e, const std::string& key, const int64_t goal) {
	if (key == "humn") {
		return goal;
	} else {
		assert(e.contains(key) && std::holds_alternative<Node>(e.at(key)));
		const auto& [lhs, rhs, op] = std::get<Node>(e.at(key));
		const int64_t lhsRes = (lhs == "humn" ? -1 : std::visit(SolveVisitor{ e }, e.at(lhs)));
		const int64_t rhsRes = (rhs == "humn" ? -1 : std::visit(SolveVisitor{ e }, e.at(rhs)));
		assert((lhsRes == -1) ^ (rhsRes == -1));
		return (lhsRes == -1
			? solve(e, lhs, find(op, rhsRes, goal))
			: solve(e, rhs, find(lhsRes, op, goal)));
	}
}

void day21(const char* filename) {
	Expr e;
	for (const std::string& line : lines(filename)) {
		const std::string key(line, 0, 4);
		int64_t val = 0;
		const auto [ptr, ec] = std::from_chars(line.data() + 6, line.data() + line.size(), val);
		if (ptr != line.data() + 6) {
			e[key] = val;
		} else {
			e[key] = Node{ std::string(line, 6, 4),std::string(line, 13, 4),line[11] };
		}
	}
	const int64_t part1 = std::visit(EvalVisitor{ e }, e.at("root"));
	// Ignore root operation, it's supposed to be = after all
	const auto& [lhs, rhs, _] = std::get<Node>(e.at("root"));
	const int64_t lhsRes = std::visit(SolveVisitor{ e }, e.at(lhs));
	const int64_t rhsRes = std::visit(SolveVisitor{ e }, e.at(rhs));
	assert((lhsRes == -1) ^ (rhsRes == -1));
	const int64_t part2 = (lhsRes == -1 ? solve(e, lhs, rhsRes) : solve(e, rhs, lhsRes));
	fmt::print("{} {}\n", part1, part2);
}

int main21() {
	day21("input/21test.txt"); // 152 301
	day21("input/21full.txt"); // 93813115694560 3910938071092
	return 0;
}
