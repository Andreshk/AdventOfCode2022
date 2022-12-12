#include "Lines.h"
#include "IntPairs.h"
#include <fmt/core.h>
#include <vector>
#include <queue>
#include <ranges> // std::ranges::find_if, std::views::as_rvalue
#include <optional>

// Intentionally copy the map (!) it will be overwritten to track visited positions
int find(std::vector<std::string> map, const IntPair start, const std::optional<IntPair> end) {
	const int n = int(map.size());
	const int m = int(map[0].size());
	auto reachable = [](char x, char y) { return (y <= x + 1); };
	std::queue<IntPair> curr, next; // current & next BFS levels, curr becomes next when exhausted
	int currLevel = 0;
	curr.push(start);
	// Replace start, end with actual heights and mark start as visited immediately
	if (end) {
		map[start.i][start.j] = -'a';
		map[end->i][end->j] = 'z';
	} else {
		map[start.i][start.j] = -'z';
	}
	while (true) {
		if (curr.empty()) {
			std::swap(curr, next);
			++currLevel;
		}
		if (curr.empty()) { // end not reachabled from start, return infinity
			return INT_MAX;
		}
		const auto [x, y] = curr.front(); curr.pop();
		const char c = -map[x][y];
		assert(std::islower(c)); // Position has been marked on queue insertion
		if (end ? x == end->i && y == end->j : map[x][y] == -'a') {
			break;
		}
		const IntPair offsets[4] = { {1,0},{0,1},{-1,0},{0,-1} };
		for (const auto [dx, dy] : offsets) {
			if (x+dx < 0 || x+dx >= n || y+dy < 0 || y+dy >= m || map[x+dx][y+dy] < 0 /*marked*/) {
				continue;
			}
			const bool reach = (end ? reachable(c, map[x+dx][y+dy]) : reachable(map[x+dx][y+dy], c));
			if (reach) {
				next.push({ x+dx,y+dy });
				map[x+dx][y+dy] *= -1;
			}
		}
	}
	return currLevel;
}

void day12(const char* filename) {
	std::vector<std::string> map{ std::from_range, std::views::as_rvalue(lines(filename)) };
	IntPair start, end;
	start.i = std::distance(map.begin(), std::ranges::find_if(map, [](const std::string& line) { return line.contains('S'); }));
	start.j = map[start.i].find('S');
	end.i = std::distance(map.begin(), std::ranges::find_if(map, [](const std::string& line) { return line.contains('E'); }));
	end.j = map[end.i].find('E');
	const int part1 = find(map, start, end);
	map[start.i][start.j] = 'a'; // 'S' position is irrelevant when going down, replace with actual height
	const int part2 = find(map, end, {}); // No specific target
	/*const int part2 = std::ranges::min(
		IntPairs(map.size(), map[0].size())
		| std::views::filter([&](const IntPair& p) { return map[p.i][p.j] == 'a'; })
		| std::views::transform([&](const IntPair& p) { return find(map, p, end); }));*/
	fmt::print("{} {}\n", part1, part2);
}

int main12() {
	day12("input/12test.txt"); // 31 29
	day12("input/12full.txt"); // 440 439
	return 0;
}