#pragma once

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <string>
#include <unordered_map>

class ContextManager
{
private:
    std::unordered_map<std::string, void *> map;

public:
    ContextManager() = default;

    template <class T>
    T *get_by_id(std::string id)
    {
        return static_cast<T*>(map[id]);
    }

    template <class T>
    void add_with_id(std::string key, T *value)
    {
        map.insert(std::pair<std::string, T*>(key, value));
    }
};

ContextManager context;

#endif