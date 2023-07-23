#include "../vmake.hpp"
using namespace std;

int main() {
	vmake::outputln(cout, " ", vmake::discard_n(vmake::range(1, 10), 2));
}
