#include <gtest/gtest.h>
#include "../src/irc.hpp"
#include "limits.h"

// ----- VALID CASES -----
TEST(ValidatePortTest, MinimumValidPort)
{
    EXPECT_EQ(validatePort("1"), 1);
}

TEST(ValidatePortTest, MaximumValidPort)
{
    EXPECT_EQ(validatePort("65535"), 65535);
}

TEST(ValidatePortTest, TypicalValidPort)
{
    EXPECT_EQ(validatePort("8080"), 8080);
}

TEST(ValidatePortTest, LeadingZeros)
{
    EXPECT_EQ(validatePort("00080"), 80);
}

// ----- INVALID RANGE -----
TEST(ValidatePortTest, AlphanumericString)
{
    EXPECT_THROW(validatePort("80port"), std::invalid_argument); // stoi parses up to 'p'
}

TEST(ValidatePortTest, SpecialCharactersAfterNumber)
{
    EXPECT_THROW(validatePort("80!"), std::invalid_argument); // stoi parses up to '!'
}

TEST(ValidatePortTest, ZeroPort)
{
    EXPECT_THROW(validatePort("0"), std::out_of_range);
}

TEST(ValidatePortTest, AboveMaxPort)
{
    EXPECT_THROW(validatePort("65536"), std::out_of_range);
}

TEST(ValidatePortTest, VeryLargeNumber)
{
    EXPECT_THROW(validatePort("9999999999"), std::out_of_range);
}

// ----- INVALID FORMATS -----
TEST(ValidatePortTest, NegativePort)
{
    EXPECT_THROW(validatePort("-1"), std::invalid_argument);
}

TEST(ValidatePortTest, AboveMaxPortWithText)
{
    EXPECT_THROW(validatePort("65536abc"), std::invalid_argument);
}

TEST(ValidatePortTest, OverflowWithText)
{
    EXPECT_THROW(validatePort("9999999999abc"), std::invalid_argument);
}

TEST(ValidatePortTest, LeadingSpace)
{
    EXPECT_THROW(validatePort(" 80"), std::invalid_argument);
}

TEST(ValidatePortTest, TrailingSpace)
{
    EXPECT_THROW(validatePort("80 "), std::invalid_argument);
}

TEST(ValidatePortTest, EmptyString)
{
    EXPECT_THROW(validatePort(""), std::invalid_argument);
}

TEST(ValidatePortTest, NonNumericString)
{
    EXPECT_THROW(validatePort("abc"), std::invalid_argument);
}

TEST(ValidatePortTest, SpecialCharactersOnly)
{
    EXPECT_THROW(validatePort("!@#"), std::invalid_argument);
}

TEST(ValidatePortTest, SpacesOnly)
{
    EXPECT_THROW(validatePort("   "), std::invalid_argument);
}

// ----- OVERFLOW -----
TEST(ValidatePortTest, IntegerOverflowPositive)
{
    EXPECT_THROW(validatePort(std::to_string(static_cast<long long>(INT_MAX) + 1)), std::out_of_range);
}

//this is due to the "-"  sign with the negative number, which is treated as a character, thus throwing the invalid_argument error
TEST(ValidatePortTest, IntegerOverflowNegative)
{
    EXPECT_THROW(validatePort(std::to_string(static_cast<long long>(INT_MIN) - 1)), std::invalid_argument);
}

// ----- WHITESPACES VARIANTS -----
TEST(ValidatePortTest, TabBeforeNumber)
{
    EXPECT_THROW(validatePort("\t123"), std::invalid_argument);
}

TEST(ValidatePortTest, NewlineBeforeNumber)
{
    EXPECT_THROW(validatePort("\n456"), std::invalid_argument);
}

TEST(ValidatePortTest, MixedWhitespaceBeforeNumber)
{
    EXPECT_THROW(validatePort(" \t\n789"), std::invalid_argument);
}

TEST(ValidatePortTest, LeadingZerosWithTextAfter)
{
    EXPECT_THROW(validatePort("00065535abc"), std::invalid_argument);
}

TEST(ValidatePortTest, LeadingZerosWithInvalidPortNumber)
{
    EXPECT_THROW(validatePort("00065536abc"), std::invalid_argument);
}

TEST(ValidatePortTest, LeadingZerosPortTooSmall)
{
    EXPECT_THROW(validatePort("0000"), std::out_of_range);
}

// ----- WHITESPACE TRIMMING IN STRING -----
TEST(ValidatePortTest, LeadingAndTrailingWhitespace)
{
    EXPECT_THROW(validatePort(" \t\n 123 \n\t "), std::invalid_argument);
}
