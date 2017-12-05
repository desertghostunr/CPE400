/**
 * @file    ThreadSafeObject.cpp
 * 
 * @brief   Implementation file for the ThreadSafeObject class
 * 
 * @author  Andrew Frost, Richard Millar
 * @version 1.00
 */

#include "ThreadSafeObject.h"

ThreadSafeObject::ThreadSafeObject() : mutex()
{

}

ThreadSafeObject::~ThreadSafeObject()
{

}

void ThreadSafeObject::getLock() 
{
    mutex.lock();
}


void ThreadSafeObject::releaseLock()
{
    mutex.unlock();
}