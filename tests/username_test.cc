#include <gtest/gtest.h>
#include "../src/irc.hpp"

// ---------- Valid cases ----------

TEST(ValidateClientNameTest, ValidSimpleName) {
    EXPECT_NO_THROW(validateClientName("Alice123"));
}

TEST(ValidateClientNameTest, ValidWithUnderscoreAndDash) {
    EXPECT_NO_THROW(validateClientName("John_Doe-99"));
}

TEST(ValidateClientNameTest, ValidMixedCase) {
    EXPECT_NO_THROW(validateClientName("CoolUser42"));
}

// ---------- Empty string ----------

TEST(ValidateClientNameTest, EmptyName) {
    EXPECT_THROW(validateClientName(""), std::invalid_argument);
}

// ---------- Invalid starting characters ----------

TEST(ValidateClientNameTest, StartsWithDollar) {
    EXPECT_THROW(validateClientName("$user"), std::invalid_argument);
}

TEST(ValidateClientNameTest, StartsWithColon) {
    EXPECT_THROW(validateClientName(":user"), std::invalid_argument);
}

TEST(ValidateClientNameTest, StartsWithAmpersand) {
    EXPECT_THROW(validateClientName("&user"), std::invalid_argument);
}

TEST(ValidateClientNameTest, StartsWithAtSymbol) {
    EXPECT_THROW(validateClientName("@user"), std::invalid_argument);
}

TEST(ValidateClientNameTest, StartsWithPlus) {
    EXPECT_THROW(validateClientName("+user"), std::invalid_argument);
}

// ---------- Invalid characters anywhere ----------

TEST(ValidateClientNameTest, ContainsSpace) {
    EXPECT_THROW(validateClientName("hello world"), std::invalid_argument);
}

TEST(ValidateClientNameTest, ContainsDot) {
    EXPECT_THROW(validateClientName("john.doe"), std::invalid_argument);
}

TEST(ValidateClientNameTest, ContainsComma) {
    EXPECT_THROW(validateClientName("name,last"), std::invalid_argument);
}

TEST(ValidateClientNameTest, ContainsExclamation) {
    EXPECT_THROW(validateClientName("what!now"), std::invalid_argument);
}

TEST(ValidateClientNameTest, ContainsQuestionMark) {
    EXPECT_THROW(validateClientName("who?me"), std::invalid_argument);
}

TEST(ValidateClientNameTest, ContainsAtInMiddle) {
    EXPECT_THROW(validateClientName("user@domain"), std::invalid_argument);
}

// ---------- Check exception message ----------

TEST(ValidateClientNameTest, MessageForStartingSymbol) {
    try {
        validateClientName(":bad");
        FAIL() << "Expected std::invalid_argument";
    } catch (const std::invalid_argument& e) {
        EXPECT_NE(std::string(e.what()).find(":"), std::string::npos);
    }
}

TEST(ValidateClientNameTest, MessageForInvalidChar) {
    try {
        validateClientName("bad,name");
        FAIL() << "Expected std::invalid_argument";
    } catch (const std::invalid_argument& e) {
        EXPECT_NE(std::string(e.what()).find(","), std::string::npos);
    }
}

TEST(ValidateClientNameTest, MessageForEmpty) {
    try {
        validateClientName("");
        FAIL() << "Expected std::invalid_argument";
    } catch (const std::invalid_argument& e) {
        EXPECT_NE(std::string(e.what()).find("empty"), std::string::npos);
    }
}
