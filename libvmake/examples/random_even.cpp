#include "../vmake.hpp"
#include <iostream>
using namespace std;

int main() {
	vmake::outputln_n(cout, 1000, " ", vmake::filter(
		vmake::rng::uniform_ints(vmake::require_unique, 1, 10000),
		[](auto x) { return x % 2 == 0; }));
}

