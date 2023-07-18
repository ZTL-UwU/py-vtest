#include "../vmake.hpp"
#include <vector>
using namespace std;

int main() {
	vmake::output(cout, " ", vmake::take(vmake::rng::uniform_ints(1, 10), 10));
	return 0;
}
