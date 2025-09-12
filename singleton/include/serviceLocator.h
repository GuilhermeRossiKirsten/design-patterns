#pragma once
#include <string>
#include <map>
#include <memory>

template<typename T>
class ServiceLocator {
private:

    //Poniteiro único para a instância única
    static std::unique_ptr<ServiceLocator> instance;
    std::map<std::string, std::unique_ptr<T>> services;

    ServiceLocator() = default; // construtor privado
    
public:

    static ServiceLocator& getInstance() {
        if (!instance) {
            instance.reset(new ServiceLocator());
        }
        return *instance;
    }

    void registerService(const std::string& name, std::unique_ptr<T> service) {
        services[name] = std::move(service);
    }

    T* getService(const std::string& name) {
        auto it = services.find(name);
        if (it != services.end()) {
            return it->second.get();
        }
        throw std::runtime_error("Service not found");
    }
};

// Definição obrigatória do membro estático do template
template<typename T>
std::unique_ptr<ServiceLocator<T>> ServiceLocator<T>::instance = nullptr;