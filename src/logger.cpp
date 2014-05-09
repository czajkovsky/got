#include "logger.h"

#include <string>
#include <ostream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "sizes.h"
#include "communicator.h"
#include "time_point.h"

void Logger::Log(MESSAGE_LEVEL message_level, const std::string& msg) {
  output_stream_ << std::left
    << TimePoint::Now() << " "
    << Message_level_to_string(message_level) << " "
    << "[" << std::setw(DIGITS_IN_RANK) << std::setfill('0')
    << std::right << Communicator::Get_rank() << "] "
    << msg
    << std::endl;
}

Logger::Logger() 
  : output_stream_(std::cout),
    DIGITS_IN_RANK(ceil(log10(Sizes::MAX_NUMBER_OF_THIEVES))) {
}

std::string Logger::Message_level_to_string(MESSAGE_LEVEL message_level) {
  std::string translated_level;
  switch(message_level) {
  case INFO:
    translated_level = " INFO";
    break;
  case WARN:
    translated_level = " WARN";
    break;
  case ERROR:
    translated_level = "ERROR";
    break;
  case DEBUG:
    translated_level = "DEBUG";
    break;
  default:
    translated_level = "-----";
  }
  return translated_level;
}
