#include "../vmake.hpp"
#include <vector>
using namespace std;

int main() {
	vmake::outputln(cout, " ", vmake::concat(vmake::range(1, 3)
		, vmake::range(1, 4), vmake::range(1, 4, 2)));
	return 0;
}
