#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "bco.h"

namespace bco_tests {

	TEST_CLASS(MyTests)
	{
	public:

		TEST_METHOD(TestOne)
		{
			bc_orbiter::SwitchStop s(0.5, []() {});

			s.func_();
		}
	};
}