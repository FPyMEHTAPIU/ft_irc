#include <gtest/gtest.h>
#include "../src/logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

class LoggerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        testLogFile = "test_log.txt";
        // Clean up any existing test file
        if (std::filesystem::exists(testLogFile))
        {
            std::filesystem::remove(testLogFile);
        }
    }

    void TearDown() override
    {
        // Clean up test file after each test
        if (std::filesystem::exists(testLogFile))
        {
            std::filesystem::remove(testLogFile);
        }
    }

    std::string testLogFile;

    std::string readFileContents(const std::string &filename)
    {
        std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
};

// ---------- Constructor Tests ----------
TEST_F(LoggerTest, DefaultConstructor)
{
    Logger logger;
    // Should create logger with console output enabled, file output disabled
    logger.info(SERVER, "Test message");
    // No assertion needed - just ensure no crash
}

TEST_F(LoggerTest, FileConstructor)
{
    Logger logger(testLogFile);
    logger.info(SERVER, "Test file message");

    // Check that file was created and contains message
    EXPECT_TRUE(std::filesystem::exists(testLogFile));
    std::string content = readFileContents(testLogFile);
    EXPECT_TRUE(content.find("Test file message") != std::string::npos);
}

TEST_F(LoggerTest, FullConstructor)
{
    Logger logger(testLogFile, false, true); // console off, file on
    logger.info(SERVER, "Test full constructor");

    EXPECT_TRUE(std::filesystem::exists(testLogFile));
    std::string content = readFileContents(testLogFile);
    EXPECT_TRUE(content.find("Test full constructor") != std::string::npos);
}

// ---------- Log Level Tests ----------
TEST_F(LoggerTest, DebugLevel)
{
    Logger logger(testLogFile);
    logger.debug(SERVER, "Debug message");

    std::string content = readFileContents(testLogFile);
    EXPECT_TRUE(content.find("[" ANSI_PURPLE "DEBUG" ANSI_RESET "]") != std::string::npos);
    EXPECT_TRUE(content.find("Debug message") != std::string::npos);
}

TEST_F(LoggerTest, InfoLevel)
{
    Logger logger(testLogFile);
    logger.info(CLIENT, "Info message");

    std::string content = readFileContents(testLogFile);
    EXPECT_TRUE(content.find("[" ANSI_BLUE "INFO" ANSI_RESET "]") != std::string::npos);
    EXPECT_TRUE(content.find("Info message") != std::string::npos);
}

TEST_F(LoggerTest, WarningLevel)
{
    Logger logger(testLogFile);
    logger.warning(AUTH, "Warning message");

    std::string content = readFileContents(testLogFile);
    EXPECT_TRUE(content.find("[" ANSI_YELLOW "WARNING" ANSI_RESET "]") != std::string::npos);
    EXPECT_TRUE(content.find("Warning message") != std::string::npos);
}

TEST_F(LoggerTest, ErrorLevel)
{
    Logger logger(testLogFile);
    logger.error(NETWORK, "Error message");

    std::string content = readFileContents(testLogFile);
    EXPECT_TRUE(content.find("[" ANSI_ORANGE "ERROR" ANSI_RESET "]") != std::string::npos);
    EXPECT_TRUE(content.find("Error message") != std::string::npos);
}

TEST_F(LoggerTest, FatalLevel)
{
    Logger logger(testLogFile);
    logger.fatal(COMMAND, "Fatal message");

    std::string content = readFileContents(testLogFile);
    EXPECT_TRUE(content.find("[" ANSI_RED "FATAL" ANSI_RESET "]") != std::string::npos);
    EXPECT_TRUE(content.find("Fatal message") != std::string::npos);
}

TEST_F(LoggerTest, SuccessLevel)
{
    Logger logger(testLogFile);
    logger.success(COMMAND, "Success message");

    std::string content = readFileContents(testLogFile);
    EXPECT_TRUE(content.find("[" ANSI_GREEN "SUCCESS" ANSI_RESET "]") != std::string::npos);
    EXPECT_TRUE(content.find("Success message") != std::string::npos);
}

// ---------- Component Tests ----------
TEST_F(LoggerTest, AllComponents)
{
    Logger logger(testLogFile);

    logger.info(SERVER, "Server message");
    logger.info(CLIENT, "Client message");
    logger.info(CHANNEL, "Channel message");
    logger.info(AUTH, "Auth message");
    logger.info(COMMAND, "Command message");
    logger.info(NETWORK, "Network message");
    logger.info(PARSER, "Parser message");

    std::string content = readFileContents(testLogFile);
    EXPECT_TRUE(content.find("[SERVER]") != std::string::npos);
    EXPECT_TRUE(content.find("[CLIENT]") != std::string::npos);
    EXPECT_TRUE(content.find("[CHANNEL]") != std::string::npos);
    EXPECT_TRUE(content.find("[AUTH]") != std::string::npos);
    EXPECT_TRUE(content.find("[COMMAND]") != std::string::npos);
    EXPECT_TRUE(content.find("[NETWORK]") != std::string::npos);
    EXPECT_TRUE(content.find("[PARSER]") != std::string::npos);
}

// ---------- Configuration Tests ----------
TEST_F(LoggerTest, EnableDisableFileOutput)
{
    Logger logger;
    logger.setLogFile(testLogFile);
    logger.enableFileOutput(true);
    logger.info(SERVER, "File enabled message");

    EXPECT_TRUE(std::filesystem::exists(testLogFile));

    logger.enableFileOutput(false);
    // File should still exist but no new content should be added
    std::string content1 = readFileContents(testLogFile);
    logger.info(SERVER, "File disabled message");
    std::string content2 = readFileContents(testLogFile);

    EXPECT_EQ(content1, content2); // Content should be same
}

TEST_F(LoggerTest, SetLogFile)
{
    Logger logger;
    logger.enableFileOutput(true);
    logger.setLogFile(testLogFile);
    logger.info(SERVER, "New file message");

    EXPECT_TRUE(std::filesystem::exists(testLogFile));
    std::string content = readFileContents(testLogFile);
    EXPECT_TRUE(content.find("New file message") != std::string::npos);
}

// ---------- Timestamp Tests ----------
TEST_F(LoggerTest, TimestampFormat)
{
    std::string timestamp = Logger::getCurrentTimestamp();

    // Basic format check: YYYY-MM-DD HH:MM:SS
    EXPECT_EQ(timestamp.length(), 19);
    EXPECT_EQ(timestamp[4], '-');
    EXPECT_EQ(timestamp[7], '-');
    EXPECT_EQ(timestamp[10], ' ');
    EXPECT_EQ(timestamp[13], ':');
    EXPECT_EQ(timestamp[16], ':');
}

TEST_F(LoggerTest, LogFormatting)
{
    Logger logger(testLogFile);
    logger.info(SERVER, "Format test");

    std::string content = readFileContents(testLogFile);

    // Check that log contains all expected parts
    EXPECT_TRUE(content.find("[") != std::string::npos); // Timestamp brackets
    EXPECT_TRUE(content.find("[" ANSI_BLUE "INFO" ANSI_RESET "]") != std::string::npos);
    EXPECT_TRUE(content.find("[SERVER]") != std::string::npos);
    EXPECT_TRUE(content.find("Format test") != std::string::npos);
}

// ---------- Multiple Messages Test ----------
TEST_F(LoggerTest, MultipleMessages)
{
    Logger logger(testLogFile);

    logger.info(SERVER, "Message 1");
    logger.error(CLIENT, "Message 2");
    logger.warning(AUTH, "Message 3");

    std::string content = readFileContents(testLogFile);

    EXPECT_TRUE(content.find("Message 1") != std::string::npos);
    EXPECT_TRUE(content.find("Message 2") != std::string::npos);
    EXPECT_TRUE(content.find("Message 3") != std::string::npos);
}