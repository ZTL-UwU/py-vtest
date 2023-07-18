#include "../vmake.hpp"
#include <vector>
using namespace std;

int main() {
	vmake::outputln(cout, " ", vmake::filter(vmake::range(1, 10), [](int x) {
		return x % 2 == 0;
	}), '\n');
	vmake::outputln(cout, " ", vmake::take(vmake::filter(vmake::iota(1), [](int x) {
		return x <= 5;
	}), 5));
	return 0;
}
