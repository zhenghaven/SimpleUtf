// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <cstring>

#include <gtest/gtest.h>

#include <SimpleUtf/Utf.hpp>

#include "Utf8Map.hpp"
#include "Utf16Map.hpp"

#ifndef SIMPLEUTF_CUSTOMIZED_NAMESPACE
using namespace SimpleUtf;
#else
using namespace SIMPLEUTF_CUSTOMIZED_NAMESPACE;
#endif

namespace SimpleUtf_Test
{
	extern size_t g_numOfTestFile;
}

GTEST_TEST(TestUtf, CountTestFile)
{
	++SimpleUtf_Test::g_numOfTestFile;
}

GTEST_TEST(TestUtf, CalcNumBits)
{
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x00U)), 0);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x01U)), 1);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x02U)), 2);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x04U)), 3);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x08U)), 4);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x10U)), 5);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x20U)), 6);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x40U)), 7);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x80U)), 8);

	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x0100U)), 9);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x0200U)), 10);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x0400U)), 11);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x0800U)), 12);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x1000U)), 13);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x2000U)), 14);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x4000U)), 15);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x8000U)), 16);

	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x010000U)), 17);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x020000U)), 18);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x040000U)), 19);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x080000U)), 20);
	EXPECT_EQ(Internal::BitWidthChar(char32_t(0x100000U)), 21);
}

GTEST_TEST(TestUtf, EnsureByteSize)
{
	// unsigned

	auto tprog1 = [](){
		Internal::EnsureByteSize<1>(static_cast<uint8_t>(0xFF));
	};
	EXPECT_NO_THROW(tprog1());

	auto tprog2 = [](){
		Internal::EnsureByteSize<1>(static_cast<uint32_t>(0xFF0000));
	};
	EXPECT_THROW(tprog2(), UtfConversionException);

	// signed

	auto tprog3 = [](){
		Internal::EnsureByteSize<1>(static_cast<char>(-1));
	};
	EXPECT_NO_THROW(tprog3());

	auto tprog4 = [](){
		Internal::EnsureByteSize<1>(static_cast<int32_t>(-1));
	};
	EXPECT_THROW(tprog4(), UtfConversionException);
}

GTEST_TEST(TestUtf, CalcNumContBytes)
{
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x00U), 0);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x01U), 0);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x02U), 0);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x04U), 0);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x08U), 0);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x10U), 0);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x20U), 0);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x40U), 0);

	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x80U), 1);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x0100U), 1);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x0200U), 1);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x0400U), 1);

	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x0800U), 2);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x1000U), 2);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x2000U), 2);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x4000U), 2);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x8000U), 2);

	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x010000U), 3);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x020000U), 3);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x040000U), 3);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x080000U), 3);
	EXPECT_EQ(Internal::CalcUtf8NumContNeeded(0x100000U), 3);

	EXPECT_THROW(Internal::CalcUtf8NumContNeeded(0xD800U);,
		UtfConversionException);
	EXPECT_THROW(Internal::CalcUtf8NumContNeeded(0xDFFFU);,
		UtfConversionException);
}

GTEST_TEST(TestUtf, CodePtToUtf8)
{
	constexpr size_t maxNumErr = 10;
	constexpr size_t numMap = sizeof(gk_Utf8MapBegins) / sizeof(char32_t);
	constexpr size_t numPerMap = (sizeof(gk_Utf8Map) / (4 * sizeof(char))) / numMap;

	size_t numErr = 0;

	for (size_t i = 0; i < numMap; ++i)
	{
		char32_t begin = gk_Utf8MapBegins[i];
		for (size_t j = 0; j < numPerMap && numErr < maxNumErr; ++j)
		{
			const uint8_t* ptr = reinterpret_cast<const uint8_t*>(gk_Utf8Map[i][j]);
			size_t len = ptr[1] == 0 ? 1 : (ptr[2] == 0 ? 2 : (ptr[3] == 0 ? 3 : 4));
			std::string ref(ptr, ptr + len);
			char32_t codePt = begin + static_cast<char32_t>(j);

			std::string res;
			if (!Internal::IsValidCodePt(codePt))
			{
				EXPECT_THROW(CodePtToUtf8Once(codePt, std::back_inserter(res));, UtfConversionException);
			}
			else
			{
				CodePtToUtf8Once(codePt, std::back_inserter(res));

				if (res != ref)
				{
					++numErr;
				}

				EXPECT_EQ(ref, res);
				EXPECT_EQ(ref.size(), CodePtToUtf8OnceGetSize(codePt));
			}
		}
	}
}

GTEST_TEST(TestUtf, Utf8ToCodePt)
{
	constexpr size_t maxNumErr = 10;
	constexpr size_t numMap = sizeof(gk_Utf8MapBegins) / sizeof(char32_t);
	constexpr size_t numPerMap = (sizeof(gk_Utf8Map) / (4 * sizeof(char))) / numMap;

	size_t numErr = 0;

	for (size_t i = 0; i < numMap; ++i)
	{
		char32_t begin = gk_Utf8MapBegins[i];
		for (size_t j = 0; j < numPerMap && numErr < maxNumErr; ++j)
		{
			char32_t codePt = begin + static_cast<char32_t>(j);

			if (Internal::IsValidCodePt(codePt))
			{
				const uint8_t* ptr = reinterpret_cast<const uint8_t*>(gk_Utf8Map[i][j]);
				size_t len = ptr[1] == 0 ? 1 : (ptr[2] == 0 ? 2 : (ptr[3] == 0 ? 3 : 4));
				std::vector<uint16_t> utf8(ptr, ptr + len);

				char32_t resCodePt = 0;
				auto it = utf8.begin();
				std::tie(resCodePt, it) = Utf8ToCodePtOnce(utf8.begin(), utf8.end());

				if (codePt != resCodePt)
				{
					++numErr;
				}
				EXPECT_EQ(codePt, resCodePt);
			}
		}
	}

	for (size_t i = 0; i < numMap; ++i)
	{
		char32_t begin = gk_Utf8MapBegins[i];
		for (size_t j = 0; j < numPerMap && numErr < maxNumErr; ++j)
		{
			char32_t codePt = begin + static_cast<char32_t>(j);

			if (Internal::IsValidCodePt(codePt))
			{
				const uint8_t* ptr = reinterpret_cast<const uint8_t*>(gk_Utf8Map[i][j]);
				size_t len = ptr[1] == 0 ? 1 : (ptr[2] == 0 ? 2 : (ptr[3] == 0 ? 3 : 4));
				std::vector<int16_t> utf8(ptr, ptr + len);

				char32_t resCodePt = 0;
				auto it = utf8.begin();
				std::tie(resCodePt, it) = Utf8ToCodePtOnce(utf8.begin(), utf8.end());

				if (codePt != resCodePt)
				{
					++numErr;
				}
				EXPECT_EQ(codePt, resCodePt);
			}
		}
	}
}

GTEST_TEST(TestUtf, CodePtToUtf16)
{
	constexpr size_t maxNumErr = 10;
	constexpr size_t numMap = sizeof(gk_Utf16MapBegins) / sizeof(char32_t);
	constexpr size_t numPerMap = (sizeof(gk_Utf16Map) / (4 * sizeof(char))) / numMap;

	size_t numErr = 0;

	for (size_t i = 0; i < numMap; ++i)
	{
		char32_t begin = gk_Utf16MapBegins[i];
		for (size_t j = 0; j < numPerMap && numErr < maxNumErr; ++j)
		{
			const uint16_t* ptr = reinterpret_cast<const uint16_t*>(gk_Utf16Map[i][j]);
			size_t len = ptr[1] == 0 ? 1 : 2;
			std::u16string ref(ptr, ptr + len);
			char32_t codePt = begin + static_cast<char32_t>(j);

			std::u16string res;
			if (!Internal::IsValidCodePt(codePt))
			{
				EXPECT_THROW(CodePtToUtf16Once(codePt, std::back_inserter(res));, UtfConversionException);
			}
			else
			{
				CodePtToUtf16Once(codePt, std::back_inserter(res));

				if (res != ref)
				{
					++numErr;
				}

				EXPECT_EQ(ref, res);
				EXPECT_EQ(ref.size(), CodePtToUtf16OnceGetSize(codePt));
			}
		}
	}
}

GTEST_TEST(TestUtf, Utf16ToCodePt)
{
	constexpr size_t maxNumErr = 10;
	constexpr size_t numMap = sizeof(gk_Utf16MapBegins) / sizeof(char32_t);
	constexpr size_t numPerMap = (sizeof(gk_Utf16Map) / (4 * sizeof(char))) / numMap;

	size_t numErr = 0;

	for (size_t i = 0; i < numMap; ++i)
	{
		char32_t begin = gk_Utf16MapBegins[i];
		for (size_t j = 0; j < numPerMap && numErr < maxNumErr; ++j)
		{
			char32_t codePt = begin + static_cast<char32_t>(j);

			if (Internal::IsValidCodePt(codePt))
			{
				const uint16_t* ptr = reinterpret_cast<const uint16_t*>(gk_Utf16Map[i][j]);
				size_t len = ptr[1] == 0 ? 1 : 2;
				//std::vector<uint16_t> utf8(ptr, ptr + strLen);

				char32_t resCodePt = 0;
				auto it = ptr;
				std::tie(resCodePt, it) = Utf16ToCodePtOnce(ptr, ptr + len);

				if (codePt != resCodePt)
				{
					++numErr;
				}
				EXPECT_EQ(codePt, resCodePt);
			}
		}
	}

	for (size_t i = 0; i < numMap; ++i)
	{
		char32_t begin = gk_Utf16MapBegins[i];
		for (size_t j = 0; j < numPerMap && numErr < maxNumErr; ++j)
		{
			char32_t codePt = begin + static_cast<char32_t>(j);

			if (Internal::IsValidCodePt(codePt))
			{
				const uint16_t* ptr = reinterpret_cast<const uint16_t*>(gk_Utf16Map[i][j]);
				size_t len = ptr[1] == 0 ? 1 : 2;
				std::vector<int32_t> utf16(ptr, ptr + len);

				char32_t resCodePt = 0;
				auto it = utf16.begin();
				std::tie(resCodePt, it) = Utf16ToCodePtOnce(utf16.begin(), utf16.end());

				if (codePt != resCodePt)
				{
					++numErr;
				}
				EXPECT_EQ(codePt, resCodePt);
			}
		}
	}
}

GTEST_TEST(TestUtf, CodePtToUtf32)
{
	constexpr size_t maxNumErr = 10;
	constexpr size_t numMap = sizeof(gk_Utf16MapBegins) / sizeof(char32_t);
	constexpr size_t numPerMap = (sizeof(gk_Utf16Map) / (4 * sizeof(char))) / numMap;

	size_t numErr = 0;

	for (size_t i = 0; i < numMap; ++i)
	{
		char32_t begin = gk_Utf16MapBegins[i];
		for (size_t j = 0; j < numPerMap && numErr < maxNumErr; ++j)
		{
			char32_t codePt = begin + static_cast<char32_t>(j);

			std::u32string res;
			if (!Internal::IsValidCodePt(codePt))
			{
				EXPECT_THROW(CodePtToUtf32Once(codePt, std::back_inserter(res));, UtfConversionException);
			}
			else
			{
				CodePtToUtf32Once(codePt, std::back_inserter(res));

				if (res[0] != codePt)
				{
					++numErr;
				}

				EXPECT_EQ(codePt, res[0]);
				EXPECT_EQ(1, CodePtToUtf32OnceGetSize(codePt));
			}
		}
	}
}

GTEST_TEST(TestUtf, Utf32ToCodePt)
{
	constexpr size_t maxNumErr = 10;
	constexpr size_t numMap = sizeof(gk_Utf16MapBegins) / sizeof(char32_t);
	constexpr size_t numPerMap = (sizeof(gk_Utf16Map) / (4 * sizeof(char))) / numMap;

	size_t numErr = 0;

	for (size_t i = 0; i < numMap; ++i)
	{
		char32_t begin = gk_Utf16MapBegins[i];
		for (size_t j = 0; j < numPerMap && numErr < maxNumErr; ++j)
		{
			char32_t codePt = begin + static_cast<char32_t>(j);

			if (Internal::IsValidCodePt(codePt))
			{
				uint64_t inArray[] = { codePt };
				char32_t resCodePt = 0;
				auto it = std::begin(inArray);
				std::tie(resCodePt, it) = Utf32ToCodePtOnce(std::begin(inArray), std::end(inArray));

				if (codePt != resCodePt)
				{
					++numErr;
				}
				EXPECT_EQ(codePt, resCodePt);
			}
		}
	}

	for (size_t i = 0; i < numMap; ++i)
	{
		char32_t begin = gk_Utf16MapBegins[i];
		for (size_t j = 0; j < numPerMap && numErr < maxNumErr; ++j)
		{
			char32_t codePt = begin + static_cast<char32_t>(j);

			if (Internal::IsValidCodePt(codePt))
			{
				int64_t inArray[] = { codePt };
				char32_t resCodePt = 0;
				auto it = std::begin(inArray);
				std::tie(resCodePt, it) = Utf32ToCodePtOnce(std::begin(inArray), std::end(inArray));

				if (codePt != resCodePt)
				{
					++numErr;
				}
				EXPECT_EQ(codePt, resCodePt);
			}
		}
	}
}

GTEST_TEST(TestUtf, Conversion)
{
	{
		const char testUtf8 [] = "\xe6\xb5\x8b\xe8\xaf\x95\xe7\xa8\x8b\xe5\xba\x8f";
		const char16_t testUtf16 [] = {0x6d4b, 0x8bd5, 0x7a0b, 0x5e8f, 0};
		const char32_t testUtf32 [] = {0x00006d4b, 0x00008bd5, 0x00007a0b, 0x00005e8f, 0};

		auto testUtf8Str = std::string(testUtf8);
		auto testUtf16Str = std::u16string(testUtf16);
		auto testUtf32Str = std::u32string(testUtf32);
		{
			// === UTF-8 --> UTF-16
			auto utf16 = Utf8ToUtf16(testUtf8);
			EXPECT_EQ(utf16, testUtf16Str);

			utf16.clear();
			EXPECT_EQ(utf16.size(), 0);
			Utf8ToUtf16(testUtf8Str.begin(), testUtf8Str.end(), std::back_inserter(utf16));
			EXPECT_EQ(utf16, testUtf16Str);
			auto utf16Size = Utf8ToUtf16GetSize(testUtf8Str.begin(), testUtf8Str.end());
			EXPECT_EQ(utf16Size, testUtf16Str.size());

			utf16.clear();
			EXPECT_EQ(utf16.size(), 0);
			auto utf16It = Utf8ToUtf16Once(std::begin(testUtf8) + 3, std::begin(testUtf8) + 6, std::back_inserter(utf16));
			EXPECT_EQ(utf16, testUtf16Str.substr(1, 1));
			EXPECT_EQ(utf16It, std::begin(testUtf8) + 6);
			std::tie(utf16Size, utf16It) = Utf8ToUtf16OnceGetSize(std::begin(testUtf8) + 3, std::begin(testUtf8) + 6);
			EXPECT_EQ(utf16Size, 1);
			EXPECT_EQ(utf16It, std::begin(testUtf8) + 6);

			// === UTF-8 --> UTF-32
			auto utf32 = Utf8ToUtf32(testUtf8);
			EXPECT_EQ(utf32, testUtf32Str);

			utf32.clear();
			EXPECT_EQ(utf32.size(), 0);
			Utf8ToUtf32(testUtf8Str.begin(), testUtf8Str.end(), std::back_inserter(utf32));
			EXPECT_EQ(utf32, testUtf32Str);
			auto utf32Size = Utf8ToUtf32GetSize(testUtf8Str.begin(), testUtf8Str.end());
			EXPECT_EQ(utf32Size, testUtf32Str.size());

			utf32.clear();
			EXPECT_EQ(utf32.size(), 0);
			auto utf32It = Utf8ToUtf32Once(std::begin(testUtf8) + 3, std::begin(testUtf8) + 6, std::back_inserter(utf32));
			EXPECT_EQ(utf32, testUtf32Str.substr(1, 1));
			EXPECT_EQ(utf32It, std::begin(testUtf8) + 6);
			std::tie(utf32Size, utf32It) = Utf8ToUtf32OnceGetSize(std::begin(testUtf8) + 3, std::begin(testUtf8) + 6);
			EXPECT_EQ(utf32Size, 1);
			EXPECT_EQ(utf32It, std::begin(testUtf8) + 6);
		}
		{
			// === UTF-16 --> UTF-8
			auto utf8 = Utf16ToUtf8(testUtf16);
			EXPECT_EQ(utf8, testUtf8Str);

			utf8.clear();
			EXPECT_EQ(utf8.size(), 0);
			Utf16ToUtf8(testUtf16Str.begin(), testUtf16Str.end(), std::back_inserter(utf8));
			EXPECT_EQ(utf8, testUtf8Str);
			auto utf8Size = Utf16ToUtf8GetSize(testUtf16Str.begin(), testUtf16Str.end());
			EXPECT_EQ(utf8Size, testUtf8Str.size());

			utf8.clear();
			EXPECT_EQ(utf8.size(), 0);
			auto utf8It = Utf16ToUtf8Once(std::begin(testUtf16) + 1, std::begin(testUtf16) + 2, std::back_inserter(utf8));
			EXPECT_EQ(utf8, testUtf8Str.substr(3, 3));
			EXPECT_EQ(utf8It, std::begin(testUtf16) + 2);
			std::tie(utf8Size, utf8It) = Utf16ToUtf8OnceGetSize(std::begin(testUtf16) + 1, std::begin(testUtf16) + 2);
			EXPECT_EQ(utf8Size, 3);
			EXPECT_EQ(utf8It, std::begin(testUtf16) + 2);

			// === UTF-16 --> UTF-32
			auto utf32 = Utf16ToUtf32(testUtf16);
			EXPECT_EQ(utf32, testUtf32Str);

			utf32.clear();
			EXPECT_EQ(utf32.size(), 0);
			Utf16ToUtf32(testUtf16Str.begin(), testUtf16Str.end(), std::back_inserter(utf32));
			EXPECT_EQ(utf32, testUtf32Str);
			auto utf32Size = Utf16ToUtf32GetSize(testUtf16Str.begin(), testUtf16Str.end());
			EXPECT_EQ(utf32Size, testUtf32Str.size());

			utf32.clear();
			EXPECT_EQ(utf32.size(), 0);
			auto utf32It = Utf16ToUtf32Once(std::begin(testUtf16) + 1, std::begin(testUtf16) + 2, std::back_inserter(utf32));
			EXPECT_EQ(utf32, testUtf32Str.substr(1, 1));
			EXPECT_EQ(utf32It, std::begin(testUtf16) + 2);
			std::tie(utf32Size, utf32It) = Utf16ToUtf32OnceGetSize(std::begin(testUtf16) + 1, std::begin(testUtf16) + 2);
			EXPECT_EQ(utf32Size, 1);
			EXPECT_EQ(utf32It, std::begin(testUtf16) + 2);
		}
		{
			// === UTF-32 --> UTF-8
			auto utf8 = Utf32ToUtf8(testUtf32);
			EXPECT_EQ(utf8, testUtf8Str);

			utf8.clear();
			EXPECT_EQ(utf8.size(), 0);
			Utf32ToUtf8(testUtf32Str.begin(), testUtf32Str.end(), std::back_inserter(utf8));
			EXPECT_EQ(utf8, testUtf8Str);
			auto utf8Size = Utf32ToUtf8GetSize(testUtf32Str.begin(), testUtf32Str.end());
			EXPECT_EQ(utf8Size, testUtf8Str.size());

			utf8.clear();
			EXPECT_EQ(utf8.size(), 0);
			auto utf8It = Utf32ToUtf8Once(std::begin(testUtf32) + 1, std::begin(testUtf32) + 2, std::back_inserter(utf8));
			EXPECT_EQ(utf8, testUtf8Str.substr(3, 3));
			EXPECT_EQ(utf8It, std::begin(testUtf32) + 2);
			std::tie(utf8Size, utf8It) = Utf32ToUtf8OnceGetSize(std::begin(testUtf32) + 1, std::begin(testUtf32) + 2);
			EXPECT_EQ(utf8Size, 3);
			EXPECT_EQ(utf8It, std::begin(testUtf32) + 2);

			// === UTF-32 --> UTF-16
			auto utf16 = Utf32ToUtf16(testUtf32);
			EXPECT_EQ(utf16, testUtf16Str);

			utf16.clear();
			EXPECT_EQ(utf16.size(), 0);
			Utf32ToUtf16(testUtf32Str.begin(), testUtf32Str.end(), std::back_inserter(utf16));
			EXPECT_EQ(utf16, testUtf16Str);
			auto utf16Size = Utf32ToUtf16GetSize(testUtf32Str.begin(), testUtf32Str.end());
			EXPECT_EQ(utf16Size, testUtf16Str.size());

			utf16.clear();
			EXPECT_EQ(utf16.size(), 0);
			auto utf16It = Utf32ToUtf16Once(std::begin(testUtf32) + 1, std::begin(testUtf32) + 2, std::back_inserter(utf16));
			EXPECT_EQ(utf16, testUtf16Str.substr(1, 1));
			EXPECT_EQ(utf16It, std::begin(testUtf32) + 2);
			std::tie(utf16Size, utf16It) = Utf32ToUtf16OnceGetSize(std::begin(testUtf32) + 1, std::begin(testUtf32) + 2);
			EXPECT_EQ(utf16Size, 1);
			EXPECT_EQ(utf16It, std::begin(testUtf32) + 2);
		}
	}
	{
		const char testUtf8 [] = "\xF0\x9F\x98\x82 \xF0\x9F\x98\x86 \xE2\x9C\x85";
		const char16_t testUtf16 [] = {0xd83d, 0xde02, 0x0020, 0xd83d, 0xde06, 0x0020, 0x2705, 0};
		const char32_t testUtf32 [] = {0x0001f602, 0x00000020, 0x0001f606, 0x00000020, 0x00002705, 0};

		auto testUtf8Str = std::string(testUtf8);
		auto testUtf16Str = std::u16string(testUtf16);
		auto testUtf32Str = std::u32string(testUtf32);
		{
			// === UTF-8 --> UTF-16
			auto utf16 = Utf8ToUtf16(testUtf8);
			EXPECT_EQ(utf16, testUtf16Str);

			utf16.clear();
			EXPECT_EQ(utf16.size(), 0);
			Utf8ToUtf16(testUtf8Str.begin(), testUtf8Str.end(), std::back_inserter(utf16));
			EXPECT_EQ(utf16, testUtf16Str);
			auto utf16Size = Utf8ToUtf16GetSize(testUtf8Str.begin(), testUtf8Str.end());
			EXPECT_EQ(utf16Size, testUtf16Str.size());

			utf16.clear();
			EXPECT_EQ(utf16.size(), 0);
			auto utf16It = Utf8ToUtf16Once(std::begin(testUtf8) + 5, std::begin(testUtf8) + 9, std::back_inserter(utf16));
			EXPECT_EQ(utf16, testUtf16Str.substr(3, 2));
			EXPECT_EQ(utf16It, std::begin(testUtf8) + 9);
			std::tie(utf16Size, utf16It) = Utf8ToUtf16OnceGetSize(std::begin(testUtf8) + 5, std::begin(testUtf8) + 9);
			EXPECT_EQ(utf16Size, 2);
			EXPECT_EQ(utf16It, std::begin(testUtf8) + 9);

			// === UTF-8 --> UTF-32
			auto utf32 = Utf8ToUtf32(testUtf8);
			EXPECT_EQ(utf32, testUtf32Str);

			utf32.clear();
			EXPECT_EQ(utf32.size(), 0);
			Utf8ToUtf32(testUtf8Str.begin(), testUtf8Str.end(), std::back_inserter(utf32));
			EXPECT_EQ(utf32, testUtf32Str);
			auto utf32Size = Utf8ToUtf32GetSize(testUtf8Str.begin(), testUtf8Str.end());
			EXPECT_EQ(utf32Size, testUtf32Str.size());

			utf32.clear();
			EXPECT_EQ(utf32.size(), 0);
			auto utf32It = Utf8ToUtf32Once(std::begin(testUtf8) + 5, std::begin(testUtf8) + 9, std::back_inserter(utf32));
			EXPECT_EQ(utf32, testUtf32Str.substr(2, 1));
			EXPECT_EQ(utf32It, std::begin(testUtf8) + 9);
			std::tie(utf32Size, utf32It) = Utf8ToUtf32OnceGetSize(std::begin(testUtf8) + 5, std::begin(testUtf8) + 9);
			EXPECT_EQ(utf32Size, 1);
			EXPECT_EQ(utf32It, std::begin(testUtf8) + 9);
		}
		{
			// === UTF-16 --> UTF-8
			auto utf8 = Utf16ToUtf8(testUtf16);
			EXPECT_EQ(utf8, testUtf8Str);

			utf8.clear();
			EXPECT_EQ(utf8.size(), 0);
			Utf16ToUtf8(testUtf16Str.begin(), testUtf16Str.end(), std::back_inserter(utf8));
			EXPECT_EQ(utf8, testUtf8Str);
			auto utf8Size = Utf16ToUtf8GetSize(testUtf16Str.begin(), testUtf16Str.end());
			EXPECT_EQ(utf8Size, testUtf8Str.size());

			utf8.clear();
			EXPECT_EQ(utf8.size(), 0);
			auto utf8It = Utf16ToUtf8Once(std::begin(testUtf16) + 3, std::begin(testUtf16) + 5, std::back_inserter(utf8));
			EXPECT_EQ(utf8, testUtf8Str.substr(5, 4));
			EXPECT_EQ(utf8It, std::begin(testUtf16) + 5);
			std::tie(utf8Size, utf8It) = Utf16ToUtf8OnceGetSize(std::begin(testUtf16) + 3, std::begin(testUtf16) + 5);
			EXPECT_EQ(utf8Size, 4);
			EXPECT_EQ(utf8It, std::begin(testUtf16) + 5);

			// === UTF-16 --> UTF-32
			auto utf32 = Utf16ToUtf32(testUtf16);
			EXPECT_EQ(utf32, testUtf32Str);

			utf32.clear();
			EXPECT_EQ(utf32.size(), 0);
			Utf16ToUtf32(testUtf16Str.begin(), testUtf16Str.end(), std::back_inserter(utf32));
			EXPECT_EQ(utf32, testUtf32Str);
			auto utf32Size = Utf16ToUtf32GetSize(testUtf16Str.begin(), testUtf16Str.end());
			EXPECT_EQ(utf32Size, testUtf32Str.size());

			utf32.clear();
			EXPECT_EQ(utf32.size(), 0);
			auto utf32It = Utf16ToUtf32Once(std::begin(testUtf16) + 3, std::begin(testUtf16) + 5, std::back_inserter(utf32));
			EXPECT_EQ(utf32, testUtf32Str.substr(2, 1));
			EXPECT_EQ(utf32It, std::begin(testUtf16) + 5);
			std::tie(utf32Size, utf32It) = Utf16ToUtf32OnceGetSize(std::begin(testUtf16) + 3, std::begin(testUtf16) + 5);
			EXPECT_EQ(utf32Size, 1);
			EXPECT_EQ(utf32It, std::begin(testUtf16) + 5);
		}
		{
			// === UTF-32 --> UTF-8
			auto utf8 = Utf32ToUtf8(testUtf32);
			EXPECT_EQ(utf8, testUtf8Str);

			utf8.clear();
			EXPECT_EQ(utf8.size(), 0);
			Utf32ToUtf8(testUtf32Str.begin(), testUtf32Str.end(), std::back_inserter(utf8));
			EXPECT_EQ(utf8, testUtf8Str);
			auto utf8Size = Utf32ToUtf8GetSize(testUtf32Str.begin(), testUtf32Str.end());
			EXPECT_EQ(utf8Size, testUtf8Str.size());

			utf8.clear();
			EXPECT_EQ(utf8.size(), 0);
			auto utf8It = Utf32ToUtf8Once(std::begin(testUtf32) + 2, std::begin(testUtf32) + 3, std::back_inserter(utf8));
			EXPECT_EQ(utf8, testUtf8Str.substr(5, 4));
			EXPECT_EQ(utf8It, std::begin(testUtf32) + 3);
			std::tie(utf8Size, utf8It) = Utf32ToUtf8OnceGetSize(std::begin(testUtf32) + 2, std::begin(testUtf32) + 3);
			EXPECT_EQ(utf8Size, 4);
			EXPECT_EQ(utf8It, std::begin(testUtf32) + 3);

			// === UTF-32 --> UTF-16
			auto utf16 = Utf32ToUtf16(testUtf32);
			EXPECT_EQ(utf16, testUtf16Str);

			utf16.clear();
			EXPECT_EQ(utf16.size(), 0);
			Utf32ToUtf16(testUtf32Str.begin(), testUtf32Str.end(), std::back_inserter(utf16));
			EXPECT_EQ(utf16, testUtf16Str);
			auto utf16Size = Utf32ToUtf16GetSize(testUtf32Str.begin(), testUtf32Str.end());
			EXPECT_EQ(utf16Size, testUtf16Str.size());

			utf16.clear();
			EXPECT_EQ(utf16.size(), 0);
			auto utf16It = Utf32ToUtf16Once(std::begin(testUtf32) + 2, std::begin(testUtf32) + 3, std::back_inserter(utf16));
			EXPECT_EQ(utf16, testUtf16Str.substr(3, 2));
			EXPECT_EQ(utf16It, std::begin(testUtf32) + 3);
			std::tie(utf16Size, utf16It) = Utf32ToUtf16OnceGetSize(std::begin(testUtf32) + 2, std::begin(testUtf32) + 3);
			EXPECT_EQ(utf16Size, 2);
			EXPECT_EQ(utf16It, std::begin(testUtf32) + 3);
		}
	}
}
