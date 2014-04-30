#ifndef THIEF_PROCESS_H_
#define THIEF_PROCESS_H_

class ThiefProcess {
public:
  static ThiefProcess& Get_process() {
    static ThiefProcess process;
    return process;
  }
  void Run(int rank, int size);

  unsigned int Increment_timestamp(unsigned int other_timestamp = 0);

  int Get_rank() const { return rank_; }
  int Get_size() const { return size_; }

private:
  ThiefProcess() {}
  ThiefProcess(const ThiefProcess&);
  ThiefProcess& operator=(const ThiefProcess&);

  unsigned int timestamp_;

  int rank_;
  int size_;
};

#endif  // THIEF_PROCESS_H_
