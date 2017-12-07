/**
 * @file    ThreadSafeObject.h
 * @brief   Definition file for the ThreadSafeObject class
 * 
 * @author  Andrew Frost, Richard Millar
 * @version 1.00
 */
#ifndef THREADSAFEOBJECT_H
#define THREADSAFEOBJECT_H

#include <mutex>

using Lock = std::unique_lock<std::mutex>;


/**
 * @brief   The ThreadSafeObject class is inherited by others to allow for objects
 *          to lock their resources between threads.
 * @details This class provides two methods, one to lock the resource, and one to
 *          release it.
 * 
 * @class   ThreadSafeObject ThreadSafeObject.h "ThreadSafeObject.h"
 */
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
