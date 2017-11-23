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