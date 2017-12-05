/**
 * @file    ThreadSafeObject.h
 * 
 * @brief   Definition file for the ThreadSafeObject class
 * @details This class is extended by the CentralComputeNode and Vehicle classes
 *          to provide a unique lock and functionality per object.
 * 
 * @author  Andrew Frost, Richard Millar
 * @version 1.00
 */
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
