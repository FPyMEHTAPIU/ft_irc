#include <gtest/gtest.h>
#include "../src/irc.hpp"
#include "../src/validation/validation.hpp"

// ---------- Valid cases ----------
TEST(IsValidChannelNameTest, SimpleValidName)
{
    EXPECT_TRUE(Validation::isValidChannelName("#general"));
}

TEST(IsValidChannelNameTest, SingleLetterName)
{
    EXPECT_TRUE(Validation::isValidChannelName("#a"));
}

TEST(IsValidChannelNameTest, MixedCaseAllowed)
{
    EXPECT_TRUE(Validation::isValidChannelName("#ChatRoom"));
}

TEST(IsValidChannelNameTest, NumbersAllowed)
{
    EXPECT_TRUE(Validation::isValidChannelName("#room42"));
}

TEST(IsValidChannelNameTest, SpecialCharsAllowed)
{
    EXPECT_TRUE(Validation::isValidChannelName("#topic-sub_topic"));
}

// ---------- Invalid: Empty or missing # ----------
TEST(IsValidChannelNameTest, EmptyName)
{
    EXPECT_FALSE(Validation::isValidChannelName(""));
}

TEST(IsValidChannelNameTest, NoHashPrefix)
{
    EXPECT_FALSE(Validation::isValidChannelName("general"));
}

TEST(IsValidChannelNameTest, OnlyHash)
{
    EXPECT_FALSE(Validation::isValidChannelName("#"));
}

// ---------- Invalid characters ----------
TEST(IsValidChannelNameTest, ContainsSpace)
{
    EXPECT_FALSE(Validation::isValidChannelName("#chat room"));
}

TEST(IsValidChannelNameTest, ContainsBell)
{
    EXPECT_FALSE(Validation::isValidChannelName(std::string("#bell") + '\a'));
}

TEST(IsValidChannelNameTest, ContainsComma)
{
    EXPECT_FALSE(Validation::isValidChannelName("#comma,here"));
}

// ---------- Edge cases ----------
TEST(IsValidChannelNameTest, MultipleHashesAllowed)
{
    EXPECT_TRUE(Validation::isValidChannelName("##meta"));
}

TEST(IsValidChannelNameTest, TrailingSpace)
{
    EXPECT_FALSE(Validation::isValidChannelName("#room "));
}

TEST(IsValidChannelNameTest, VeryLongName)
{
    std::string longName = "#" + std::string(200, 'a');
    EXPECT_TRUE(Validation::isValidChannelName(longName)); // currently passes, but may be restricted later
}

TEST(IsValidChannelNameTest, UppercaseAndLowercase)
{
    EXPECT_TRUE(Validation::isValidChannelName("#TeStChAnNeL"));
}
