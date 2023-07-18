#include "../vmake.hpp"
#include <vector>
using namespace std;

int main() {
	vmake::output(cout, " ", vmake::take(vmake::rng::uniform_ints(1, 10), 15));
	cout << endl;
	vmake::output(cout, " ", vmake::take(vmake::rng::uniform_reals(1., 10.), 5));
	return 0;
}
