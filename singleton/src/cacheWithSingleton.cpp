#include "cacheWithSingleton.h"

CacheWithSingleton::CacheWithSingleton() = default;

void CacheWithSingleton::put(const std::string& key, const std::string& value) {
    cache[key] = value;
}

std::string CacheWithSingleton::get(const std::string& key) {
    auto it = cache.find(key);
    return it != cache.end() ? it->second : "";
}

void CacheWithSingleton::remove(const std::string& key) {
    cache.erase(key);
}

void CacheWithSingleton::clear() {
    cache.clear();
}
