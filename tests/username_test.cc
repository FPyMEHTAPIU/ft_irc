#include <gtest/gtest.h>
#include "../src/irc.hpp"
#include "../src/validation/validation.hpp"

// ---------- Valid cases ----------

TEST(ValidateClientNameTest, ValidSimpleName)
{
    EXPECT_NO_THROW(Validation::validateClientName("Alice123"));
}

TEST(ValidateClientNameTest, ValidWithUnderscoreAndDash)
{
    EXPECT_NO_THROW(Validation::validateClientName("John_Doe-99"));
}

TEST(ValidateClientNameTest, ValidMixedCase)
{
    EXPECT_NO_THROW(Validation::validateClientName("CoolUser42"));
}

// ---------- Empty string ----------

TEST(ValidateClientNameTest, EmptyName)
{
    EXPECT_THROW(Validation::validateClientName(""), std::invalid_argument);
}

// ---------- Invalid starting characters ----------

TEST(ValidateClientNameTest, StartsWithDollar)
{
    EXPECT_THROW(Validation::validateClientName("$user"), std::invalid_argument);
}

TEST(ValidateClientNameTest, StartsWithColon)
{
    EXPECT_THROW(Validation::validateClientName(":user"), std::invalid_argument);
}

TEST(ValidateClientNameTest, StartsWithAmpersand)
{
    EXPECT_THROW(Validation::validateClientName("&user"), std::invalid_argument);
}

TEST(ValidateClientNameTest, StartsWithAtSymbol)
{
    EXPECT_THROW(Validation::validateClientName("@user"), std::invalid_argument);
}

TEST(ValidateClientNameTest, StartsWithPlus)
{
    EXPECT_THROW(Validation::validateClientName("+user"), std::invalid_argument);
}

// ---------- Invalid characters anywhere ----------

TEST(ValidateClientNameTest, ContainsSpace)
{
    EXPECT_THROW(Validation::validateClientName("hello world"), std::invalid_argument);
}

TEST(ValidateClientNameTest, ContainsDot)
{
    EXPECT_THROW(Validation::validateClientName("john.doe"), std::invalid_argument);
}

TEST(ValidateClientNameTest, ContainsComma)
{
    EXPECT_THROW(Validation::validateClientName("name,last"), std::invalid_argument);
}

TEST(ValidateClientNameTest, ContainsExclamation)
{
    EXPECT_THROW(Validation::validateClientName("what!now"), std::invalid_argument);
}

TEST(ValidateClientNameTest, ContainsQuestionMark)
{
    EXPECT_THROW(Validation::validateClientName("who?me"), std::invalid_argument);
}

TEST(ValidateClientNameTest, ContainsAtInMiddle)
{
    EXPECT_THROW(Validation::validateClientName("user@domain"), std::invalid_argument);
}

// ---------- Check exception message ----------

TEST(ValidateClientNameTest, MessageForStartingSymbol)
{
    try
    {
        Validation::validateClientName(":bad");
        FAIL() << "Expected std::invalid_argument";
    }
    catch (const std::invalid_argument &e)
    {
        EXPECT_NE(std::string(e.what()).find(":"), std::string::npos);
    }
}

TEST(ValidateClientNameTest, MessageForInvalidChar)
{
    try
    {
        Validation::validateClientName("bad,name");
        FAIL() << "Expected std::invalid_argument";
    }
    catch (const std::invalid_argument &e)
    {
        EXPECT_NE(std::string(e.what()).find(","), std::string::npos);
    }
}

TEST(ValidateClientNameTest, MessageForEmpty)
{
    try
    {
        Validation::validateClientName("");
        FAIL() << "Expected std::invalid_argument";
    }
    catch (const std::invalid_argument &e)
    {
        EXPECT_NE(std::string(e.what()).find("empty"), std::string::npos);
    }
}
// ---------- Boundary length tests ----------
TEST(ValidateClientNameTest, SingleCharacterName)
{
    EXPECT_NO_THROW(Validation::validateClientName("A"));
}

// ---------- Case sensitivity and allowed chars ----------
TEST(ValidateClientNameTest, AllUppercase)
{
    EXPECT_NO_THROW(Validation::validateClientName("USERNAME"));
}

TEST(ValidateClientNameTest, AllLowercase)
{
    EXPECT_NO_THROW(Validation::validateClientName("username"));
}

TEST(ValidateClientNameTest, UnderscoreOnlyInside)
{
    EXPECT_NO_THROW(Validation::validateClientName("user_name"));
}

TEST(ValidateClientNameTest, DashOnlyInside)
{
    EXPECT_NO_THROW(Validation::validateClientName("user-name"));
}

// ---------- Multiple invalid conditions ----------
TEST(ValidateClientNameTest, StartsWithInvalidAndContainsSpace)
{
    EXPECT_THROW(Validation::validateClientName(":user name"), std::invalid_argument);
}

// ---------- Exception message checks ----------

TEST(ValidateClientNameTest, MessageForAsterisk)
{
    try
    {
        Validation::validateClientName("user*name");
        FAIL() << "Expected std::invalid_argument";
    }
    catch (const std::invalid_argument &e)
    {
        EXPECT_NE(std::string(e.what()).find("*"), std::string::npos);
    }
}

TEST(ValidateClientNameTest, MessageForDot)
{
    try
    {
        Validation::validateClientName("evil.dot");
        FAIL() << "Expected std::invalid_argument";
    }
    catch (const std::invalid_argument &e)
    {
        EXPECT_NE(std::string(e.what()).find("."), std::string::npos);
    }
}

// ---------- IRC forbidden chars ----------
TEST(ValidateClientNameTest, ContainsAsterisk)
{
    EXPECT_THROW(Validation::validateClientName("user*name"), std::invalid_argument);
}

TEST(ValidateClientNameTest, ContainsExclamationMark)
{
    EXPECT_THROW(Validation::validateClientName("user!name"), std::invalid_argument);
}

TEST(ValidateClientNameTest, ContainsAtSignAnywhere)
{
    EXPECT_THROW(Validation::validateClientName("user@name"), std::invalid_argument);
}

// ---------- Soft rule: dot ----------
TEST(ValidateClientNameTest, ContainsDotShouldBeDiscouraged)
{
    EXPECT_THROW(Validation::validateClientName("john.doe"), std::invalid_argument);
}
