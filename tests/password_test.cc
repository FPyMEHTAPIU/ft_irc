#include <gtest/gtest.h>
#include "../src/irc.hpp"
#include "../src/validation/validation.hpp"
#include "../src/common/constants.hpp"

TEST(ValidatePasswordTest, EmptyPassword)
{
    EXPECT_THROW(Validation::validatePassword(""), std::out_of_range);
}

TEST(ValidatePasswordTest, TooShortPassword)
{
    EXPECT_THROW(Validation::validatePassword("123"), std::out_of_range);
}

TEST(ValidatePasswordTest, TooLongPassword)
{
    EXPECT_THROW(Validation::validatePassword(std::string(PASSWORD_MAX_LENGTH + 1, 'a')), std::out_of_range);
}

TEST(ValidatePasswordTest, ContainsWhitespace)
{
    EXPECT_THROW(Validation::validatePassword("pass word"), std::invalid_argument);
    EXPECT_THROW(Validation::validatePassword("password\t123"), std::invalid_argument);
}

TEST(ValidatePasswordTest, ValidPassword)
{
    EXPECT_NO_THROW(Validation::validatePassword("validPass1"));
    EXPECT_NO_THROW(Validation::validatePassword(std::string(PASSWORD_MIN_LENGTH, 'a')));
    EXPECT_NO_THROW(Validation::validatePassword(std::string(PASSWORD_MAX_LENGTH, 'a')));
}

TEST(ValidatePasswordTest, OnlySpaces)
{
    EXPECT_THROW(Validation::validatePassword("     "), std::invalid_argument);
}

TEST(ValidatePasswordTest, SpaceAtStart)
{
    EXPECT_THROW(Validation::validatePassword(" pass"), std::invalid_argument);
}

TEST(ValidatePasswordTest, SpaceAtEnd)
{
    EXPECT_THROW(Validation::validatePassword("pass "), std::invalid_argument);
}

TEST(ValidatePasswordTest, TabAtStart)
{
    EXPECT_THROW(Validation::validatePassword("\tpassword"), std::invalid_argument);
}

TEST(ValidatePasswordTest, TabAtEnd)
{
    EXPECT_THROW(Validation::validatePassword("password\t"), std::invalid_argument);
}

TEST(ValidatePasswordTest, NewlineCharacter)
{
    EXPECT_THROW(Validation::validatePassword("pass\nword"), std::invalid_argument);
}

TEST(ValidatePasswordTest, PasswordExactlyMinLength)
{
    EXPECT_NO_THROW(Validation::validatePassword(std::string(PASSWORD_MIN_LENGTH, 'a')));
}

TEST(ValidatePasswordTest, PasswordExactlyMaxLength)
{
    EXPECT_NO_THROW(Validation::validatePassword(std::string(PASSWORD_MAX_LENGTH, 'a')));
}

TEST(ValidatePasswordTest, PasswordOneLessThanMin)
{
    EXPECT_THROW(Validation::validatePassword(std::string(PASSWORD_MIN_LENGTH - 1, 'a')), std::out_of_range);
}

TEST(ValidatePasswordTest, PasswordOneMoreThanMax)
{
    EXPECT_THROW(Validation::validatePassword(std::string(PASSWORD_MAX_LENGTH + 1, 'a')), std::out_of_range);
}

TEST(ValidatePasswordTest, SpecialCharactersAllowed)
{
    EXPECT_NO_THROW(Validation::validatePassword("P@ssw0rd!"));
}

TEST(ValidatePasswordTest, OnlyNumbers)
{
    EXPECT_NO_THROW(Validation::validatePassword("12345678"));
}
