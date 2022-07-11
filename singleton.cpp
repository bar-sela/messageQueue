//// inspired from : https://gist.github.com/daniebker/2299755
#include<stdio.h>
#include <assert.h>
#include <mutex>

std::mutex lock;
template<typename T>
class Singleton
{
public:
    static T* getInstance();
    static void destroy();

private:
    Singleton(Singleton const&){};


protected:
    static T* instance;

    Singleton(){ instance = static_cast <T*> (this); };
    ~Singleton(){};
};

template<typename T>
T* Singleton<T>::instance = 0;

template<typename T>
T* Singleton<T>::getInstance()
{
    lock.lock();
    if(!instance)
    {
        Singleton<T>::instance = new T();
    }
    lock.unlock();
    return instance;
}

template<typename T>
void Singleton<T>::destroy()
{
    lock.lock();
    delete Singleton<T>::instance;
    Singleton<T>::instance = 0;
    lock.unlock();
}

int main()
{
    FILE* d1 = Singleton<FILE>::getInstance();
    FILE* d2 = Singleton<FILE>::getInstance();
    assert(d1 == d2);

}