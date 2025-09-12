#include <concepts>
#include <iostream>
#include <memory>
#include "serviceLocator.h"
#include "cacheWithSingleton.h"

template<typename T>
concept Cache = requires(T cache, std::string key, std::string value) {
    { cache.put(key, value) } -> std::same_as<void>;
    { cache.get(key) } -> std::same_as<std::string>;
    { cache.remove(key) } -> std::same_as<void>;
    { cache.clear() } -> std::same_as<void>;
};


int main() {
    static_assert(Cache<CacheWithSingleton>, "CacheWithSingleton does not satisfy the cache concept");

    auto& serviceLocator = ServiceLocator<CacheWithSingleton>::getInstance();
    serviceLocator.registerService("cache", std::make_unique<CacheWithSingleton>());
    auto cache1 = serviceLocator.getService("cache");
    auto cache2 = serviceLocator.getService("cache");
    // auto cache3 = serviceLocator.getService("nonexistent");

    cache1->put("key1", "value1");
    std::cout << cache2->get("key1") << std::endl;
    // std::cout << cache3->get("key1") << std::endl;

    std::cout << std::boolalpha;
    std::cout << (cache1 == cache2) << std::endl; // print 1 (true)


    std::cout << "Cache with Singleton Pattern implemented successfully." << std::endl;
    return 0;
}