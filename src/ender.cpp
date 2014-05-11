#include "ender.h"

#include <signal.h>

#include "logger.h"

Ender::Ender()
  : end_(false) {
  signal(SIGUSR1, Ender::Signal_handler);
}

void Ender::Signal_handler(int signum) {
  LOG_WARN("Received signal(" << signum << ")")
  Ender::Get_ender().end_ = true;
}
