// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <gtest/gtest.h>

namespace SimpleUtf_Test
{
	size_t g_numOfTestFile = 0;
}

int main(int argc, char** argv)
{
	constexpr size_t EXPECTED_NUM_OF_TEST_FILE = 1;

	std::cout << "===== SimpleUtf test program =====" << std::endl;
	std::cout << std::endl;

	std::cout << "EXPECTED_NUM_OF_TEST_FILE = " << EXPECTED_NUM_OF_TEST_FILE << std::endl;
	std::cout << "__cplusplus = " << __cplusplus << std::endl;

	std::cout << std::endl;
	std::cout << "===== SimpleUtf test start   =====" << std::endl;

	testing::InitGoogleTest(&argc, argv);
	int testRet = RUN_ALL_TESTS();

	if (SimpleUtf_Test::g_numOfTestFile != EXPECTED_NUM_OF_TEST_FILE)
	{
		std::cout << "********************************************************************************" << std::endl;
		std::cout << "***** WARNING: Expecting " << EXPECTED_NUM_OF_TEST_FILE;
		std::cout << " testing source files, but only ";
		std::cout << SimpleUtf_Test::g_numOfTestFile << " were ran. *****" << std::endl;
		std::cout << "********************************************************************************" << std::endl;

		return -1;
	}

	return testRet;
}
