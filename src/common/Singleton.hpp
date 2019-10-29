#ifndef Singleton_H
#define Singleton_H

template<typename T>
class Singleton
{
    public:
    static T* getInstance()
    {
      static T thesingleton;
      return &thesingleton;
    }
};

#endif // Singleton_H
