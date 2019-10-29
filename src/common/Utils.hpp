#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <math.h>
#include <sys/time.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>

using namespace std::chrono;

inline uint64_t getTimestamp()
{
    std::chrono::time_point<std::chrono::system_clock> ts = std::chrono::system_clock::now();
    return uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(ts.time_since_epoch()).count());
}
inline std::string getTimestampStr()
{
    std::chrono::time_point<std::chrono::system_clock> ts = std::chrono::system_clock::now();
    uint64_t timestamp = uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(ts.time_since_epoch()).count());
    return std::to_string(timestamp);
}
inline std::time_t TimestampToTime_t(uint64_t timestamp)
{
    std::chrono::milliseconds milliseconds(timestamp);
    std::chrono::time_point<std::chrono::system_clock> time_point(milliseconds);
    return std::chrono::system_clock::to_time_t(time_point);
}
inline std::string TimestampToFormattedString(uint64_t timestamp){
    std::stringstream ss;
    auto in_time_t = TimestampToTime_t(timestamp);
    ss <<  std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%X");
    ss << "." << (timestamp % 1000);
    return ss.str();
}

// template class to provide singleton functionality
/*template<typename T> class Singleton
{
    public: static T* get()
    {
      static T thesingleton;
      return &thesingleton;
    }
};*/


class ToString {
  std::ostringstream stream;
public:
  template<typename T>
  inline ToString &operator<<(const T&val) {
    stream << val;
    return *this;
  }

  inline operator std::string() const {
    return stream.str();
  }
};

//class ExecutionTimer
//{
//private:
//    std::string name;
//    const steady_clock::time_point mStart = steady_clock::now();
//public:
//    ExecutionTimer(std::string _name = "") : name(_name) { }

//    ~ExecutionTimer()
//    {
//        const auto end = steady_clock::now();
//        std::cout << this->name << " Destructor Elapsed: "
//                  << duration<double, std::nano> (end - mStart).count()
//                  << " ns" << std::endl;
//    }

//    inline void stop()
//    {
//        const auto end = steady_clock::now();
//        std::cout << this->name << " Stop Elapsed: "
//                  << duration<double, std::nano> (end - mStart).count()
//                  << " ns" << std::endl;
//    }
//};

inline double roundDouble(double value, int precision = 2)
{
    double multiplier = 1;
    for(int i=0; i<precision; i++){ multiplier *= 10; }
    return trunc(value*multiplier)/multiplier;
}


inline uint32_t getTickCount()
{
        struct timeval tv;
        if(gettimeofday(&tv, NULL) != 0)
                return 0;

        return (uint32_t)((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

#endif // UTILS_H
