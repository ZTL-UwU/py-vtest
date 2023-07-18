#include "../vmake.hpp"
#include <vector>
using namespace std;

int main() {
	vmake::outputln(cout, " ", vmake::take(vmake::rng::uniform_ints(1, 10), 15));
	vmake::outputln(cout, " ", vmake::take(vmake::rng::uniform_reals(1., 10.), 5));
	return 0;
}
