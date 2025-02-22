#pragma once

#include <sstream>
#include <iostream>
#include <cstdlib>

namespace ErrorHandler {
    inline std::string FormatError(const std::string &message, const std::string &file, const std::string &function,
                                   int line) {
        std::ostringstream oss;
        oss << "[ERROR](" << function << "()," << file << "#" << line << "): " << message << std::endl;

        return oss.str();
    }

    inline std::string FormatWarning(const std::string &message, const std::string &file, const std::string &function,
                                     int line) {
        std::ostringstream oss;
        oss << "[WARN](" << function << "()," << file << "#" << line << "): " << message << std::endl;

        return oss.str();
    }

    inline void ThrowError(const std::string &message, const std::string &file, const std::string &function, int line) {
        std::cerr << FormatError(message, file, function, line);
        std::exit(EXIT_FAILURE);
    }

    inline void Warn(const std::string &message, const std::string &file, const std::string &function, int line) {
        std::cerr << FormatWarning(message, file, function, line);
    }
}

