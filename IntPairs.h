#pragma once
#include <ranges> // std::random_access_range
#include <iterator> // std::random_access_iterator_tag
#include <cassert>

// The value, returned from dereferencing an IntPairs::iterator
// Intentionally minimalistic as possible - best used via structured bindings immediately after introducing.
struct IntPair { int i, j; };

// Range of all integer pairs (i,j) such that 0<=i<n and 0<=j<n, ordered lexicographically for convenience.
// Fully models the random_access_range concept - to be used in standard ranges algorithms.
// Can also be used as for (const auto [i,j] : IntPairs{ n,m }) { ... }
// Homebrew equivalent to cartesian_product(iota(0,n),iota(0,m)), except simpler & cheaper to compile :)
class IntPairs {
	int n = 0;
	int m = 0;
public:
	IntPairs(int n, int m) : n{ n }, m{ m } {}
	class iterator;
	iterator begin() const { return { n,m,false }; }
	iterator end() const { return { n,m,true }; }
	int size() const { return n * m; }
	bool empty() const { return (n == 0 || m == 0); }
	// Iterator to model the std::random_access_iterator concept
	class iterator {
		int n, m, i, j;
		friend IntPairs;
		iterator(int n, int m, bool end) : n{ n }, m{ m }, i{ end ? n : 0 }, j{ 0 } {}
	public:
		iterator() : iterator(0, 0, true) {}
		using value_type        = IntPair;
		using reference         = IntPair;
		using difference_type   = int;
		using iterator_category = std::random_access_iterator_tag;
		// Everything below is _required_ for this iterator category:
		value_type operator*() const {
			assert(i >= 0 && i < n && j >= 0 && j < m);
			return { i,j };
		}
		iterator& operator+=(const difference_type k) {
			const int newOff = i * m + j + k;
			i = newOff / m;
			j = newOff % m;
			return *this;
		}
		iterator& operator-=(const difference_type k) { return (*this -= (-k)); }
		iterator operator+(const difference_type k) const { return iterator{ *this } += k; }
		iterator operator-(const difference_type k) const { return iterator{ *this } -= k; }
		reference operator[](const difference_type k) const { return *(*this + k); }
		friend difference_type operator-(const iterator& lhs, const iterator& rhs) {
			assert(lhs.n == rhs.n && lhs.m == rhs.m);
			return (lhs.i - rhs.i) * lhs.m + (lhs.j - rhs.j);
		}
		friend iterator operator+(const difference_type k, const iterator& it) { return (it + k); }
		iterator& operator++() {
			assert(i < n);
			if (++j == m) {
				++i;
				j = 0;
			}
			return *this;
		}
		iterator& operator--() {
			if (j-- == 0) {
				--i;
				j = m - 1;
			}
			return *this;
		}
		iterator operator++(int) { auto copy = *this; ++*this; return copy; }
		iterator operator--(int) { auto copy = *this; --*this; return copy; }
		friend bool operator==(const iterator& lhs, const iterator& rhs) {
			assert(lhs.n == rhs.n && lhs.m == rhs.m);
			return (lhs.i == rhs.i && lhs.j == rhs.j);
		}
		// Faster than auto-generating operator<=> from ((lhs.i * lhs.m + lhs.j) <=> (rhs.i * rhs.m + rhs.j))
		friend bool operator<(const iterator& lhs, const iterator& rhs) {
			assert(lhs.n == rhs.n && lhs.m == rhs.m);
			return (lhs.i < rhs.i || (lhs.i == rhs.i && lhs.j < rhs.j));
		}
		friend bool operator>(const iterator& lhs, const iterator& rhs) { return (rhs < lhs); }
		friend bool operator<=(const iterator& lhs, const iterator& rhs) { return !(rhs < lhs); }
		friend bool operator>=(const iterator& lhs, const iterator& rhs) { return !(lhs < rhs); }
	};
};
// Sanity check
static_assert(std::ranges::random_access_range<IntPairs>);
