#include <gtest/gtest.h>
#include "../src/irc.hpp"
#include "../src/validation/validation.hpp"

TEST(ValidateCommandTest, AcceptsAllValidCommandsLowercase)
{
    EXPECT_NO_THROW(Validation::validateCommand("pass"));
    EXPECT_NO_THROW(Validation::validateCommand("nick"));
    EXPECT_NO_THROW(Validation::validateCommand("user"));
    EXPECT_NO_THROW(Validation::validateCommand("join"));
    EXPECT_NO_THROW(Validation::validateCommand("privmsg"));
    EXPECT_NO_THROW(Validation::validateCommand("mode"));
    EXPECT_NO_THROW(Validation::validateCommand("invite"));
    EXPECT_NO_THROW(Validation::validateCommand("kick"));
    EXPECT_NO_THROW(Validation::validateCommand("topic"));
    EXPECT_NO_THROW(Validation::validateCommand("msg"));
    EXPECT_NO_THROW(Validation::validateCommand("ping"));
}

TEST(ValidateCommandTest, AcceptsAllValidCommandsUppercase)
{
    EXPECT_NO_THROW(Validation::validateCommand("PASS"));
    EXPECT_NO_THROW(Validation::validateCommand("NICK"));
    EXPECT_NO_THROW(Validation::validateCommand("USER"));
    EXPECT_NO_THROW(Validation::validateCommand("JOIN"));
    EXPECT_NO_THROW(Validation::validateCommand("PRIVMSG"));
    EXPECT_NO_THROW(Validation::validateCommand("MODE"));
    EXPECT_NO_THROW(Validation::validateCommand("INVITE"));
    EXPECT_NO_THROW(Validation::validateCommand("KICK"));
    EXPECT_NO_THROW(Validation::validateCommand("TOPIC"));
    EXPECT_NO_THROW(Validation::validateCommand("MSG"));
    EXPECT_NO_THROW(Validation::validateCommand("PING"));
}

TEST(ValidateCommandTest, AcceptsAllValidCommandsMixedCase)
{
    EXPECT_NO_THROW(Validation::validateCommand("PaSs"));
    EXPECT_NO_THROW(Validation::validateCommand("NiCk"));
    EXPECT_NO_THROW(Validation::validateCommand("UsEr"));
    EXPECT_NO_THROW(Validation::validateCommand("JoIn"));
    EXPECT_NO_THROW(Validation::validateCommand("PriVMsg"));
    EXPECT_NO_THROW(Validation::validateCommand("MoDe"));
    EXPECT_NO_THROW(Validation::validateCommand("InViTe"));
    EXPECT_NO_THROW(Validation::validateCommand("KiCk"));
    EXPECT_NO_THROW(Validation::validateCommand("ToPiC"));
    EXPECT_NO_THROW(Validation::validateCommand("MsG"));
    EXPECT_NO_THROW(Validation::validateCommand("pInG"));
}

TEST(ValidateCommandTest, RejectsInvalidStrings)
{
    EXPECT_THROW(Validation::validateCommand(""), std::invalid_argument);
    EXPECT_THROW(Validation::validateCommand("abc"), std::invalid_argument);
    EXPECT_THROW(Validation::validateCommand("123"), std::invalid_argument);
    EXPECT_THROW(Validation::validateCommand("passs"), std::invalid_argument); // extra letter
    EXPECT_THROW(Validation::validateCommand("nickk"), std::invalid_argument);
    EXPECT_THROW(Validation::validateCommand("priv msg"), std::invalid_argument); // space in middle
}

TEST(ValidateCommandTest, RejectsPartialMatches)
{
    EXPECT_THROW(Validation::validateCommand("pri"), std::invalid_argument);
    EXPECT_THROW(Validation::validateCommand("jo"), std::invalid_argument);
    EXPECT_THROW(Validation::validateCommand("top"), std::invalid_argument);
    EXPECT_THROW(Validation::validateCommand("nam"), std::invalid_argument);
}

TEST(ValidateCommandTest, RejectsWithWhitespace)
{
    EXPECT_THROW(Validation::validateCommand(" pass"), std::invalid_argument);
    EXPECT_THROW(Validation::validateCommand("pass "), std::invalid_argument);
    EXPECT_THROW(Validation::validateCommand("  join"), std::invalid_argument);
    EXPECT_THROW(Validation::validateCommand("kick  "), std::invalid_argument);
}

TEST(ValidateCommandTest, RejectsWithSpecialCharacters)
{
    EXPECT_THROW(Validation::validateCommand("pass!"), std::invalid_argument);
    EXPECT_THROW(Validation::validateCommand("@nick"), std::invalid_argument);
    EXPECT_THROW(Validation::validateCommand("user#"), std::invalid_argument);
    EXPECT_THROW(Validation::validateCommand("join$"), std::invalid_argument);
}
