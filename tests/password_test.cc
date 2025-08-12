#include <gtest/gtest.h>
#include "../src/irc.hpp"

TEST(ValidatePasswordTest, EmptyPassword) {
    EXPECT_THROW(validatePassword(""), std::out_of_range);
}

TEST(ValidatePasswordTest, TooShortPassword) {
    EXPECT_THROW(validatePassword("123"), std::out_of_range);
}

TEST(ValidatePasswordTest, TooLongPassword) {
    EXPECT_THROW(validatePassword(std::string(PWD_MAX_LENGTH + 1, 'a')), std::out_of_range);
}

TEST(ValidatePasswordTest, ContainsWhitespace) {
    EXPECT_THROW(validatePassword("pass word"), std::invalid_argument);
    EXPECT_THROW(validatePassword("password\t123"), std::invalid_argument);
}

TEST(ValidatePasswordTest, ValidPassword) {
    EXPECT_NO_THROW(validatePassword("validPass1"));
    EXPECT_NO_THROW(validatePassword(std::string(PWD_MIN_LENGTH, 'a')));
    EXPECT_NO_THROW(validatePassword(std::string(PWD_MAX_LENGTH, 'a')));
}

TEST(ValidatePasswordTest, OnlySpaces) {
    EXPECT_THROW(validatePassword("     "), std::invalid_argument);
}

TEST(ValidatePasswordTest, SpaceAtStart) {
    EXPECT_THROW(validatePassword(" pass"), std::invalid_argument);
}

TEST(ValidatePasswordTest, SpaceAtEnd) {
    EXPECT_THROW(validatePassword("pass "), std::invalid_argument);
}

TEST(ValidatePasswordTest, TabAtStart) {
    EXPECT_THROW(validatePassword("\tpassword"), std::invalid_argument);
}

TEST(ValidatePasswordTest, TabAtEnd) {
    EXPECT_THROW(validatePassword("password\t"), std::invalid_argument);
}

TEST(ValidatePasswordTest, NewlineCharacter) {
    EXPECT_THROW(validatePassword("pass\nword"), std::invalid_argument); 
}

TEST(ValidatePasswordTest, PasswordExactlyMinLength) {
    EXPECT_NO_THROW(validatePassword(std::string(PWD_MIN_LENGTH, 'a')));
}

TEST(ValidatePasswordTest, PasswordExactlyMaxLength) {
    EXPECT_NO_THROW(validatePassword(std::string(PWD_MAX_LENGTH, 'a')));
}

TEST(ValidatePasswordTest, PasswordOneLessThanMin) {
    EXPECT_THROW(validatePassword(std::string(PWD_MIN_LENGTH - 1, 'a')), std::out_of_range);
}

TEST(ValidatePasswordTest, PasswordOneMoreThanMax) {
    EXPECT_THROW(validatePassword(std::string(PWD_MAX_LENGTH + 1, 'a')), std::out_of_range);
}

TEST(ValidatePasswordTest, SpecialCharactersAllowed) {
    EXPECT_NO_THROW(validatePassword("P@ssw0rd!"));
}

TEST(ValidatePasswordTest, OnlyNumbers) {
    EXPECT_NO_THROW(validatePassword("12345678"));
}
