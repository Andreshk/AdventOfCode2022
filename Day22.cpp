#include "Lines.h"
#include "IntPairs.h"
#include <fmt/core.h>
#include <charconv>

// All 4 functions take an edge position & return the new position+direction pair

//     1
// 2 3 4
//     5 6
// Simple wrap-around in the opposite direction on the example input
std::tuple<IntPair, int> testWrap(const IntPair pos) {
	if (pos.i == 0) { // Up from 1
		return { {11,pos.j},3 };
	} else if (pos.j == 8 && pos.i < 4) { // Left from 1
		return { {pos.i,11},2 };
	} else if (pos.i == 4 && pos.j < 4) { // Up from 2
		return { {7,pos.j},3 };
	} else if (pos.i == 4 && pos.j >= 4 && pos.j < 8) { // Up from 3
		return { {7,pos.j},3 };
	} else if (pos.j == 0) { // Left from 2
		return { {pos.i,11},2 };
	} else if (pos.i == 7 && pos.j < 4) { // Down from 2
		return { {4,pos.j},1 };
	} else if (pos.i == 7 && pos.j >= 4 && pos.j < 8) { // Down from 3
		return { {4,pos.j},1 };
	} else if (pos.j == 8 && pos.i >= 8) { // Left from 5
		return { {pos.i,15},2 };
	} else if (pos.i == 11 && pos.j < 12) { // Down from 5
		return { {0,pos.j},1 };
	} else if (pos.i == 11 && pos.j >= 12) { // Down from 6
		return { {8,pos.j},1 };
	} else if (pos.j == 15) { // Right from 6
		return { {pos.i,8},0 };
	} else if (pos.i == 8) { // Up from 6
		return { {11,pos.j},3 };
	} else if (pos.j == 11 && pos.i >= 4 && pos.i < 8) { // Right from 4
		return { {pos.i,0},0 };
	} else if (pos.j == 11 && pos.i < 4) { // Right from 1
		return { {pos.i,8},0 };
	}
	assert(false); return {};
}

//     1
// 2 3 4
//     5 6
// Wrap according to the folded example input
std::tuple<IntPair,int> testFold(const IntPair pos) {
	if (pos.i == 0) { // Up from 1
		return { {4,11 - pos.j},1 };
	} else if (pos.j == 8 && pos.i < 4) { // Left from 1
		return { {8,4 + pos.i},1 };
	} else if (pos.i == 4 && pos.j < 4) { // Up from 2
		return { {0,11 - pos.j},1 };
	} else if (pos.i == 4 && pos.j >= 4 && pos.j < 8) { // Up from 3
		return { {pos.j - 4,8},0 };
	} else if (pos.j == 0) { // Left from 2
		return { {11,11 - pos.i},3 };
	} else if (pos.i == 7 && pos.j < 4) { // Down from 2
		return { {11,15 - pos.i},2 };
	} else if (pos.i == 7 && pos.j >= 4 && pos.j < 8) { // Down from 3
		return { {8 + 11 - pos.j,8},0 };
	} else if (pos.j == 8 && pos.i >= 8) { // Left from 5
		return { {7,19 - pos.i},3 };
	} else if (pos.i == 11 && pos.j < 12) { // Down from 5
		return { {7,11 - pos.j},3 };
	} else if (pos.i == 11 && pos.j >= 12) { // Down from 6
		return { {15 - pos.j,0},0 };
	} else if (pos.j == 15) { // Right from 6
		return { {11 - pos.i,11},2 };
	} else if (pos.i == 8) { // Up from 6
		return { {19 - pos.j,11},2 };
	} else if (pos.j == 11 && pos.i >= 4 && pos.i < 8) { // Right from 4
		return { {8,19 - pos.i},1 };
	} else if (pos.j == 11 && pos.i < 4) { // Right from 1
		return { {15 - pos.i,15},2 };
	}
	assert(false); return {};
}

//   1 2
//   3
// 4 5
// 6
// Simple wrap-around in the opposite direction on _MY_ full input
std::tuple<IntPair, int> fullWrap(const IntPair pos) {
	if (pos.i == 0 && pos.j < 100) { // Up from 1
		return { {149,pos.j},3 };
	} else if (pos.j == 50 && pos.i < 50) { // Left from 1
		return { {pos.i,149},2 };
	} else if (pos.j == 50 && pos.i >= 50 && pos.i < 100) { // Left from 3
		return { {pos.i,99},2 };
	} else if (pos.i == 100 && pos.j < 50) { // Up from 4
		return { {199,pos.j},3 };
	} else if (pos.j == 0 && pos.i < 150) { // Left from 4
		return { {pos.i,99},2 };
	} else if (pos.j == 0 && pos.i >= 150) { // Left from 6
		return { {pos.i,49},2 };
	} else if (pos.i == 199) { // Down from 6
		return { {100,pos.j},1 };
	} else if (pos.j == 49 && pos.i >= 150) { // Right from 6
		return { {pos.i,0},0 };
	} else if (pos.i == 149 && pos.j >= 50) { // Down from 5
		return { {0,pos.j},1 };
	} else if (pos.j == 99 && pos.i >= 100) { // Right from 5
		return { {pos.i,0},0 };
	} else if (pos.j == 99 && pos.i >= 50 && pos.i < 100) { // Right from 3
		return { {pos.i,50},0 };
	} else if (pos.i == 49 && pos.j > 100) { // Down from 2
		return { {0,pos.j},1 };
	} else if (pos.j == 149) { // Right from 2
		return { {pos.i,50},0 };
	} else if (pos.i == 0 && pos.j >= 100) { // Up from 2
		return { {49,pos.j},3 };
	}
	assert(false); return {};
}

//   1 2
//   3
// 4 5
// 6
// Wrap according to _MY_ folded full input
std::tuple<IntPair, int> fullFold(const IntPair pos) {
	if (pos.i == 0 && pos.j < 100) { // Up from 1
		return { {150 + pos.j - 50,0},0 };
	} else if (pos.j == 50 && pos.i < 50) { // Left from 1
		return { {149 - pos.i,0},0 };
	} else if (pos.j == 50 && pos.i >= 50 && pos.i < 100) { // Left from 3
		return { {100,pos.i - 50},1 };
	} else if (pos.i == 100 && pos.j < 50) { // Up from 4
		return { {50 + pos.j,50},0 };
	} else if (pos.j == 0 && pos.i < 150) { // Left from 4
		return { {49 - (pos.i - 100),50},0 };
	} else if (pos.j == 0 && pos.i >= 150) { // Left from 6
		return { {0,50 + (pos.i - 150)},1 };
	} else if (pos.i == 199) { // Down from 6
		return { {0,100 + pos.j},1 };
	} else if (pos.j == 49 && pos.i >= 150) { // Right from 6
		return { {149,50 + (pos.i - 150)},3 };
	} else if (pos.i == 149 && pos.j >= 50) { // Down from 5
		return { {150 + (pos.j - 50),49},2 };
	} else if (pos.j == 99 && pos.i >= 100) { // Right from 5
		return { {149 - pos.i,149},2 };
	} else if (pos.j == 99 && pos.i >= 50 && pos.i < 100) { // Right from 3
		return { {49,50 + pos.i},3 };
	} else if (pos.i == 49 && pos.j > 100) { // Down from 2
		return { {pos.j - 50,99},2 };
	} else if (pos.j == 149) { // Right from 2
		return { {149 - pos.i,99},2 };
	} else if (pos.i == 0 && pos.j >= 100) { // Up from 2
		return { {199, pos.j - 100},3 };
	}
	assert(false); return {};
}

int day22(const char* filename, std::tuple<IntPair, int>(*wrapFn)(const IntPair)) {
	std::vector<std::string> map{ std::from_range, std::views::as_rvalue(lines(filename)) };
	const std::string path = std::move(map.back());
	map.pop_back(); map.pop_back();
	auto valid = [&](IntPair pos) {
		return (pos.i >= 0 && pos.i < map.size() && pos.j >= 0 && pos.j < map[pos.i].size() && map[pos.i][pos.j] != ' ');
	};
	// Directions, in following order: R D L U
	// After left rotation: U R D L; After right rotation: D L U R
	const IntPair dp[4] = { {0,1},{1,0},{0,-1},{-1,0} };
	IntPair pos = { 0,int(std::ranges::find(map.front(),'.') - map.front().begin()) };
	int dir = 0; // Start off going right
	const char* from = path.data();
	const char* to = from + path.size();
	while (from != to) {
		int steps = 0;
		from = std::from_chars(from, to, steps).ptr;
		for (int i = 0; i < steps; ++i) {
			IntPair next = pos + dp[dir];
			int nextDir = dir;
			if (!valid(next)) {
				std::tie(next, nextDir) = wrapFn(pos);
			}
			assert(valid(next));
			if (map[next.i][next.j] != '#') {
				pos = next;
				dir = nextDir;
			} else {
				break;
			}
		}
		// No rotation after the last move
		if (from != to) {
			const char rot = *from++;
			dir = (rot == 'L' ? (dir + 3) % 4 : (dir + 1) % 4);
		}
	}
	return 1000 * (pos.i + 1) + 4 * (pos.j + 1) + dir;
}

int main22() {
	fmt::print("{}\n", day22("input/22test.txt", testWrap)); // 6032
	fmt::print("{}\n", day22("input/22full.txt", fullWrap)); // 103224
	fmt::print("{}\n", day22("input/22test.txt", testFold)); // 5031
	fmt::print("{}\n", day22("input/22full.txt", fullFold)); // 189097
	return 0;
}