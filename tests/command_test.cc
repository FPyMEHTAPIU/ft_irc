#include <gtest/gtest.h>
#include "../src/irc.hpp"

TEST(ValidateCommandTest, AcceptsAllValidCommandsLowercase) {
    EXPECT_NO_THROW(validateCommand("pass"));
    EXPECT_NO_THROW(validateCommand("nick"));
    EXPECT_NO_THROW(validateCommand("user"));
    EXPECT_NO_THROW(validateCommand("join"));
    EXPECT_NO_THROW(validateCommand("part"));
    EXPECT_NO_THROW(validateCommand("privmsg"));
    EXPECT_NO_THROW(validateCommand("notice"));
    EXPECT_NO_THROW(validateCommand("mode"));
    EXPECT_NO_THROW(validateCommand("invite"));
    EXPECT_NO_THROW(validateCommand("kick"));
    EXPECT_NO_THROW(validateCommand("topic"));
    EXPECT_NO_THROW(validateCommand("names"));
    EXPECT_NO_THROW(validateCommand("list"));
    EXPECT_NO_THROW(validateCommand("oper"));
    EXPECT_NO_THROW(validateCommand("kill"));
    EXPECT_NO_THROW(validateCommand("quit"));
    EXPECT_NO_THROW(validateCommand("msg"));
}

TEST(ValidateCommandTest, AcceptsAllValidCommandsUppercase) {
    EXPECT_NO_THROW(validateCommand("PASS"));
    EXPECT_NO_THROW(validateCommand("NICK"));
    EXPECT_NO_THROW(validateCommand("USER"));
    EXPECT_NO_THROW(validateCommand("JOIN"));
    EXPECT_NO_THROW(validateCommand("PART"));
    EXPECT_NO_THROW(validateCommand("PRIVMSG"));
    EXPECT_NO_THROW(validateCommand("NOTICE"));
    EXPECT_NO_THROW(validateCommand("MODE"));
    EXPECT_NO_THROW(validateCommand("INVITE"));
    EXPECT_NO_THROW(validateCommand("KICK"));
    EXPECT_NO_THROW(validateCommand("TOPIC"));
    EXPECT_NO_THROW(validateCommand("NAMES"));
    EXPECT_NO_THROW(validateCommand("LIST"));
    EXPECT_NO_THROW(validateCommand("OPER"));
    EXPECT_NO_THROW(validateCommand("KILL"));
    EXPECT_NO_THROW(validateCommand("QUIT"));
    EXPECT_NO_THROW(validateCommand("MSG"));
}

TEST(ValidateCommandTest, AcceptsAllValidCommandsMixedCase) {
    EXPECT_NO_THROW(validateCommand("PaSs"));
    EXPECT_NO_THROW(validateCommand("NiCk"));
    EXPECT_NO_THROW(validateCommand("UsEr"));
    EXPECT_NO_THROW(validateCommand("JoIn"));
    EXPECT_NO_THROW(validateCommand("PaRt"));
    EXPECT_NO_THROW(validateCommand("PriVMsg"));
    EXPECT_NO_THROW(validateCommand("NoTiCe"));
    EXPECT_NO_THROW(validateCommand("MoDe"));
    EXPECT_NO_THROW(validateCommand("InViTe"));
    EXPECT_NO_THROW(validateCommand("KiCk"));
    EXPECT_NO_THROW(validateCommand("ToPiC"));
    EXPECT_NO_THROW(validateCommand("NaMeS"));
    EXPECT_NO_THROW(validateCommand("LiSt"));
    EXPECT_NO_THROW(validateCommand("OpEr"));
    EXPECT_NO_THROW(validateCommand("KiLl"));
    EXPECT_NO_THROW(validateCommand("QuIt"));
    EXPECT_NO_THROW(validateCommand("MsG"));
}

TEST(ValidateCommandTest, RejectsInvalidStrings) {
    EXPECT_THROW(validateCommand(""), std::invalid_argument);
    EXPECT_THROW(validateCommand("abc"), std::invalid_argument);
    EXPECT_THROW(validateCommand("123"), std::invalid_argument);
    EXPECT_THROW(validateCommand("passs"), std::invalid_argument);  // extra letter
    EXPECT_THROW(validateCommand("nickk"), std::invalid_argument);
    EXPECT_THROW(validateCommand("priv msg"), std::invalid_argument); // space in middle
}

TEST(ValidateCommandTest, RejectsPartialMatches) {
    EXPECT_THROW(validateCommand("pri"), std::invalid_argument);
    EXPECT_THROW(validateCommand("jo"), std::invalid_argument);
    EXPECT_THROW(validateCommand("top"), std::invalid_argument);
    EXPECT_THROW(validateCommand("nam"), std::invalid_argument);
}

TEST(ValidateCommandTest, RejectsWithWhitespace) {
    EXPECT_THROW(validateCommand(" pass"), std::invalid_argument);
    EXPECT_THROW(validateCommand("pass "), std::invalid_argument);
    EXPECT_THROW(validateCommand("  join"), std::invalid_argument);
    EXPECT_THROW(validateCommand("kick  "), std::invalid_argument);
}

TEST(ValidateCommandTest, RejectsWithSpecialCharacters) {
    EXPECT_THROW(validateCommand("pass!"), std::invalid_argument);
    EXPECT_THROW(validateCommand("@nick"), std::invalid_argument);
    EXPECT_THROW(validateCommand("user#"), std::invalid_argument);
    EXPECT_THROW(validateCommand("join$"), std::invalid_argument);
}
