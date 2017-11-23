#ifndef THREADSAFEOBJECT_H
#define THREADSAFEOBJECT_H

#include <mutex>

using Lock = std::unique_lock<std::mutex>;

class ThreadSafeObject 
{
public:
    ThreadSafeObject();
    ~ThreadSafeObject();

    void getLock();
    void releaseLock();

private:
    mutable std::mutex mutex;
};

#endif
