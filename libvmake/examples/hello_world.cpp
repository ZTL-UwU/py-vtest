#include "../vmake.hpp"
using namespace std;

int main() {
	vector<string> a{"HELLO", "WORLD", "!"};
	vmake::outputln(cout, " ", vmake::transform(vmake::extract(a.begin(), a.end()), [](auto &&s) {
		std::transform(s.cbegin(), s.cend(), s.begin(), [] (auto c) { return std::tolower(c); });
		return s;
	}));
}
