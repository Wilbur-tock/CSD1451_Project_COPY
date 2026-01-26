#include "DebugPrint.hpp"

namespace DebugPrint {
	void Print(double val) {
		std::cout << val << std::endl;
	}

	void Print(double val1, double val2) {
		std::cout << val1 << ' ' << val2 << std::endl;
	}

	void Print(double val1, double val2, double val3) {
		std::cout << val1 << ' ' << val2 << ' ' << val3 << std::endl;
	}
}