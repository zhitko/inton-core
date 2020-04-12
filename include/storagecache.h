#ifndef STORAGECACHE_H
#define STORAGECACHE_H

#include "IntonCore_global.h"

namespace IntonCore {

template <class T>
class StorageCache
{
public:
    StorageCache();
    StorageCache(T* value);

    T* getValue();
    void setValue(T* value);
    void setValue(T value);

    bool isExists();
    void clear();

private:
    T* value;
    bool is_exists;
};

template<class T>
StorageCache<T>::StorageCache():
    is_exists(false),
    value(nullptr)
{

}

template<class T>
StorageCache<T>::StorageCache(T* value):
    is_exists(true),
    value(value)
{

}

template<class T>
T* StorageCache<T>::getValue()
{
    return this->value;
}

template<class T>
void StorageCache<T>::setValue(T* value)
{
    this->value = value;
    this->is_exists = true;
}

template<class T>
void StorageCache<T>::setValue(T value)
{
    this->value = new T(value);
    this->is_exists = true;
}

template<class T>
bool StorageCache<T>::isExists()
{
    return this->is_exists;
}

template<class T>
void StorageCache<T>::clear()
{
    delete this->value;
    this->value = nullptr;
    this->is_exists = false;
}

}

#endif // STORAGECACHE_H
