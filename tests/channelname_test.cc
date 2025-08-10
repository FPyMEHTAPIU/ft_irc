#include <gtest/gtest.h>
#include "../src/irc.hpp"

// ---------- Valid cases ----------
TEST(IsValidChannelNameTest, SimpleValidName) {
    EXPECT_TRUE(isValidChannelName("#general"));
}

TEST(IsValidChannelNameTest, SingleLetterName) {
    EXPECT_TRUE(isValidChannelName("#a"));
}

TEST(IsValidChannelNameTest, MixedCaseAllowed) {
    EXPECT_TRUE(isValidChannelName("#ChatRoom"));
}

TEST(IsValidChannelNameTest, NumbersAllowed) {
    EXPECT_TRUE(isValidChannelName("#room42"));
}

TEST(IsValidChannelNameTest, SpecialCharsAllowed) {
    EXPECT_TRUE(isValidChannelName("#topic-sub_topic"));
}

// ---------- Invalid: Empty or missing # ----------
TEST(IsValidChannelNameTest, EmptyName) {
    EXPECT_FALSE(isValidChannelName(""));
}

TEST(IsValidChannelNameTest, NoHashPrefix) {
    EXPECT_FALSE(isValidChannelName("general"));
}

TEST(IsValidChannelNameTest, OnlyHash) {
    EXPECT_FALSE(isValidChannelName("#"));
}

// ---------- Invalid characters ----------
TEST(IsValidChannelNameTest, ContainsSpace) {
    EXPECT_FALSE(isValidChannelName("#chat room"));
}

TEST(IsValidChannelNameTest, ContainsBell) {
    EXPECT_FALSE(isValidChannelName(std::string("#bell") + '\a'));
}

TEST(IsValidChannelNameTest, ContainsComma) {
    EXPECT_FALSE(isValidChannelName("#comma,here"));
}

// ---------- Edge cases ----------
TEST(IsValidChannelNameTest, MultipleHashesAllowed) {
    EXPECT_TRUE(isValidChannelName("##meta"));
}

TEST(IsValidChannelNameTest, TrailingSpace) {
    EXPECT_FALSE(isValidChannelName("#room "));
}

TEST(IsValidChannelNameTest, VeryLongName) {
    std::string longName = "#" + std::string(200, 'a');
    EXPECT_TRUE(isValidChannelName(longName)); // currently passes, but may be restricted later
}

TEST(IsValidChannelNameTest, UppercaseAndLowercase) {
    EXPECT_TRUE(isValidChannelName("#TeStChAnNeL"));
}
