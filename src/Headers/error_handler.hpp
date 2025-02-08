#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

class ErrorHandler : public std::exception {
public:
  ErrorHandler(const std::string &message, const std::string &file,
               const std::string &function, int line)
      : m_message(message), m_file(file), m_function(function), m_line(line) {}

  const char *what() const noexcept override { return m_errorMessage.c_str(); }

  std::string getErrorInfo() const {
    std::ostringstream oss;

    oss << ANSI_COLOR_RED;
    oss << "[ERROR](" << m_function << "()," << m_file << "#" << m_line
        << "): " << m_message << std::endl;
    oss << ANSI_COLOR_RESET;

    return oss.str();
  }

  std::string getWarnInfo() const {
    std::ostringstream oss;

    oss << ANSI_COLOR_YELLOW;
    oss << "[WARN](" << m_function << "()," << m_file << "#" << m_line
        << "): " << m_message << std::endl;
    oss << ANSI_COLOR_RESET;

    return oss.str();
  }

  void updateErrorMessage() { m_errorMessage = getErrorInfo(); }

  void UpdateWarnMessage() { m_errorMessage = getWarnInfo(); }

  static void throwError(const std::string &message, const std::string &file,
                         const std::string &function, int line) {
    ErrorHandler err(message, file, function, line);
    err.updateErrorMessage();
    std::cerr << err.what();
    std::exit(EXIT_FAILURE);
  }

  static void warn(const std::string &message, const std::string &file,
                   const std::string &function, int line) {
    ErrorHandler err(message, file, function, line);
    err.UpdateWarnMessage();
    std::cerr << err.what();
  }

private:
  std::string m_message;
  std::string m_file;
  std::string m_function;
  int m_line;
  mutable std::string m_errorMessage;
};

#endif // ERROR_HANDLER_H
