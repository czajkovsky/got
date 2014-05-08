#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <ostream>
#include <sstream>

class Logger {
public:
  enum MESSAGE_LEVEL {
    DEBUG,
    INFO,
    WARN,
    ERROR
  };

  static Logger& Get_logger() {
    static Logger logger;
    return logger;
  }

  void Log(MESSAGE_LEVEL, const std::string& msg);

  static std::string Message_level_to_string(MESSAGE_LEVEL);

private:
  Logger();
  Logger(const Logger&);
  Logger& operator=(const Logger&);

  static std::string Get_current_time();

  std::ostream& output_stream_;

  const unsigned int DIGITS_IN_RANK;
};

#define LOG_DEBUG(msg) {\
  std::stringstream ss; ss << msg;\
  Logger::Get_logger().Log(Logger::DEBUG, ss.str());\
};

#define LOG_INFO(msg) {\
  std::stringstream ss; ss << msg;\
  Logger::Get_logger().Log(Logger::INFO, ss.str());\
};

#define LOG_WARN(msg) {\
  std::stringstream ss; ss << msg;\
  Logger::Get_logger().Log(Logger::WARN, ss.str());\
};

#define LOG_ERROR(msg) {\
  std::stringstream ss; ss << msg;\
  Logger::Get_logger().Log(Logger::ERROR, ss.str());\
};

#endif  // LOGGER_H_
