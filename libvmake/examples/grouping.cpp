#include "../vmake.hpp"
using namespace std;

int main() {
	vmake::outputln_n(cout, 5, " ", vmake::transform(vmake::group<2>(vmake::iota(1)), [](auto x) {
		return get<0>(x) * get<1>(x);
	}));
	return 0;
}
