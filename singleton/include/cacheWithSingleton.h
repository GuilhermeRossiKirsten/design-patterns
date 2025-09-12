#pragma once
#include <string>
#include <map>

class CacheWithSingleton {
private:
    std::map<std::string, std::string> cache;

public:
    CacheWithSingleton();
    CacheWithSingleton(const CacheWithSingleton&) = delete;
    CacheWithSingleton& operator=(const CacheWithSingleton&) = delete;

    void put(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void remove(const std::string& key);
    void clear();
};
