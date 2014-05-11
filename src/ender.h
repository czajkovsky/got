#ifndef ENDER_H_
#define ENDER_H_

class Ender {
public:
  static Ender& Get_ender() {
    static Ender ender;
    return ender;
  }

  bool End() { return end_; }

private:
  Ender();
  Ender(const Ender&);
  Ender& operator=(const Ender&);

  static void Signal_handler(int signum);

  bool end_;
};

#endif  // ENDER_H_
