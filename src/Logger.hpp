#pragma once

#include <string>
#include <fstream>
#include <sstream>

#define ANSI_RED "\033[0;91m"
#define ANSI_ORANGE "\e[0m\e[38;5;202m"
#define ANSI_YELLOW "\033[0;93m"
#define ANSI_GREEN "\033[0;92m"
#define ANSI_BLUE "\e[0m\e[38;5;39m"
#define ANSI_PURPLE "\e[0m\e[38;5;171m"
#define ANSI_RESET "\033[0;39m"

enum LogLevel
{
    DEBUG,
    INFO,
    SUCCESS,
    WARNING,
    ERROR,
    FATAL
};
enum Component
{
    SERVER,
    CLIENT,
    CHANNEL,
    AUTH,
    COMMAND,
    NETWORK,
    PARSER
};

class Logger
{
public:
    Logger();
    explicit Logger(const std::string &logFile);
    Logger(const std::string &logFile, bool consoleOutput, bool fileOutput);
    ~Logger();

    void setLogFile(const std::string &filename);
    void enableConsoleOutput(bool enable);
    void enableFileOutput(bool enable);

    void log(LogLevel level, Component component, const std::string &message);
    void debug(Component component, const std::string &message);
    void success(Component component, const std::string &message);
    void info(Component component, const std::string &message);
    void warning(Component component, const std::string &message);
    void error(Component component, const std::string &message);
    void fatal(Component component, const std::string &message);

    static std::string getCurrentTimestamp();

private:
    std::ofstream _logFile;
    std::string _logFileName;
    bool _consoleOutput;
    bool _fileOutput;

    void writeLog(const std::string &formattedMessage);
};