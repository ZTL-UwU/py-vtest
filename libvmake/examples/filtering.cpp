#include "../vmake.hpp"
#include <vector>
using namespace std;

int main() {
	vmake::output(cout, " ", vmake::filter(vmake::range(1, 10), [](int x) {
		return x % 2 == 0;
	}));
	cout << endl;
	vmake::output(cout, " ", vmake::take(vmake::filter(vmake::iota(1), [](int x) {
		return x <= 5;
	}), 5));
	return 0;
}
