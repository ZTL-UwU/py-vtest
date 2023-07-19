#include "../vmake.hpp"
using namespace std;

int main() {
	vmake::outputln(cout, " ", vmake::repeat_n("hello", 5));
	vmake::outputln_n(cout, 5, " ", vmake::repeat("world"));
	return 0;
}
