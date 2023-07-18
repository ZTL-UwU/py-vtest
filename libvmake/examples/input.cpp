#include "../vmake.hpp"
using namespace std;

int main() {
	int n;
	cin >> n;
	vmake::output_n(cout, n, " ", vmake::transform(vmake::extract(istream_iterator<int>(cin)), [](int x) {
		return x * x;
	}));
	return 0;
}
