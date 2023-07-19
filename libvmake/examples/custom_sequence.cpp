#include "../vmake.hpp"
using namespace std;

// equivlant to vmake::range(2, 11, 2)
struct my_sequence {
	using result = int;

	int p;
	my_sequence() : p(2) {}

	bool is_terminated() const noexcept {
		return p > 10;
	}

	int operator()() {
		p += 2;
		return p - 2;
	}
};

int main() {
	static_assert(vmake::is_sequence_t<my_sequence>::value, "my_sequence is not a valid sequence!");
	vmake::outputln(cout, " ", my_sequence{});
	vmake::outputln(cout, " ", vmake::range(2, 11, 2));
	return 0;
}
