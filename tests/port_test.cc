#include <gtest/gtest.h>
#include "../src/irc.hpp"

// ----- VALID CASES -----
TEST(ValidatePortTest, MinimumValidPort) {
    EXPECT_EQ(validatePort("1"), 1);
}

TEST(ValidatePortTest, MaximumValidPort) {
    EXPECT_EQ(validatePort("65535"), 65535);
}

TEST(ValidatePortTest, TypicalValidPort) {
    EXPECT_EQ(validatePort("8080"), 8080);
}

TEST(ValidatePortTest, LeadingZeros) {
    EXPECT_EQ(validatePort("00080"), 80);
}

TEST(ValidatePortTest, LeadingSpace) {
    EXPECT_EQ(validatePort(" 80"), 80);
}

TEST(ValidatePortTest, TrailingSpace) {
    EXPECT_EQ(validatePort("80 "), 80);
}

TEST(ValidatePortTest, AlphanumericString) {
    EXPECT_EQ(validatePort("80port"), 80); // stoi parses up to 'p'
}

TEST(ValidatePortTest, SpecialCharactersAfterNumber) {
    EXPECT_EQ(validatePort("80!"), 80); // stoi parses up to '!'
}

// ----- INVALID RANGE -----
TEST(ValidatePortTest, ZeroPort) {
    EXPECT_THROW(validatePort("0"), std::out_of_range);
}

TEST(ValidatePortTest, NegativePort) {
    EXPECT_THROW(validatePort("-1"), std::out_of_range);
}

TEST(ValidatePortTest, AboveMaxPort) {
    EXPECT_THROW(validatePort("65536"), std::out_of_range);
}

TEST(ValidatePortTest, AboveMaxPortWithText) {
    EXPECT_THROW(validatePort("65536abc"), std::out_of_range);
}

TEST(ValidatePortTest, VeryLargeNumber) {
    EXPECT_THROW(validatePort("9999999999"), std::out_of_range);
}

TEST(ValidatePortTest, OverflowWithText) {
    EXPECT_THROW(validatePort("9999999999abc"), std::out_of_range);
}

// ----- INVALID FORMATS -----
TEST(ValidatePortTest, EmptyString) {
    EXPECT_THROW(validatePort(""), std::invalid_argument);
}

TEST(ValidatePortTest, NonNumericString) {
    EXPECT_THROW(validatePort("abc"), std::invalid_argument);
}

TEST(ValidatePortTest, SpecialCharactersOnly) {
    EXPECT_THROW(validatePort("!@#"), std::invalid_argument);
}

TEST(ValidatePortTest, SpacesOnly) {
    EXPECT_THROW(validatePort("   "), std::invalid_argument);
}

// ----- OVERFLOW -----
TEST(ValidatePortTest, IntegerOverflowPositive) {
    EXPECT_THROW(validatePort(std::to_string(static_cast<long long>(INT_MAX) + 1)), std::out_of_range);
}

TEST(ValidatePortTest, IntegerOverflowNegative) {
    EXPECT_THROW(validatePort(std::to_string(static_cast<long long>(INT_MIN) - 1)), std::out_of_range);
}

// ----- WHITESPACES VARIANTS -----
TEST(ValidatePortTest, TabBeforeNumber) {
    EXPECT_EQ(validatePort("\t123"), 123);
}

TEST(ValidatePortTest, NewlineBeforeNumber) {
    EXPECT_EQ(validatePort("\n456"), 456);
}

TEST(ValidatePortTest, MixedWhitespaceBeforeNumber) {
    EXPECT_EQ(validatePort(" \t\n789"), 789);
}

TEST(ValidatePortTest, WhitespaceInsideStringInvalid) {
    // stoi stops at first invalid char, so this tries to parse "80 90" -> "80" and ignores after space,
    // but space is not a digit so parsing stops at 80
    EXPECT_EQ(validatePort("80 90"), 80);
}

TEST(ValidatePortTest, LeadingZerosWithTextAfter) {
    EXPECT_EQ(validatePort("00065535abc"), 65535);
}

TEST(ValidatePortTest, LeadingZerosWithInvalidPortNumber) {
    EXPECT_THROW(validatePort("00065536abc"), std::out_of_range);
}

TEST(ValidatePortTest, LeadingZerosPortTooSmall) {
    EXPECT_THROW(validatePort("0000"), std::out_of_range);
}

// ----- WHITESPACE TRIMMING IN STRING -----
TEST(ValidatePortTest, LeadingAndTrailingWhitespace) {
    EXPECT_EQ(validatePort(" \t\n 123 \n\t "), 123);
}
