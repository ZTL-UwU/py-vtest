#include "../vmake.hpp"
using namespace std;

int main() {
	vmake::outputln(cout, " ", vmake::transform(vmake::zip(vmake::range(1, 10)
		, vmake::range(2, 10, 2)), [](const auto &x) {
			return std::get<0>(x) + std::get<1>(x);
		}));
}
