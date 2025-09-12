#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

class paymentProcessor {
public:
    virtual void operation() = 0;
    virtual void getPaymentInfo() = 0;
    
    //Necessário para evitar memory leaks devido ao uso de polimorfismo, garantindo que o destrutor correto seja chamado
    virtual ~paymentProcessor() = default; 
};

struct PIX {};
struct MERCADOPAGO {};
struct PAGARME {};

template<typename paymentMethod>
class Product: public paymentProcessor {
public:
    Product() { std::cout << "Product generic" << std::endl; }
    ~Product() { std::cout << "Product generic destroyed" << std::endl; }
    void operation() override { std::cout << "Operation generic" << std::endl; }
    void getPaymentInfo() override { std::cout << "Payment info generic" << std::endl; }
};

template<>
class Product<PIX>: public paymentProcessor {
public:
    Product() { std::cout << "Product created with PIX" << std::endl; }
    ~Product() { std::cout << "Product PIX destroyed" << std::endl; }
    void operation() override { std::cout << "Operation PIX" << std::endl; }
    void getPaymentInfo() override { std::cout << "Payment info PIX" << std::endl; }
};

template<>
class Product<MERCADOPAGO>: public paymentProcessor {
public:
    Product() { std::cout << "Product created with MERCADOPAGO" << std::endl; }
    ~Product() { std::cout << "Product MERCADOPAGO destroyed" << std::endl; }
    void operation() override { std::cout << "Operation MERCADOPAGO" << std::endl; }
    void getPaymentInfo() override { std::cout << "Payment info MERCADOPAGO" << std::endl; }
};

template<>
class Product<PAGARME>: public paymentProcessor {
public:
    Product() { std::cout << "Product created with PAGARME" << std::endl; }
    ~Product() { std::cout << "Product PAGARME destroyed" << std::endl; }
    void operation() override { std::cout << "Operation PAGARME" << std::endl; }
    void getPaymentInfo() override { std::cout << "Payment info PAGARME" << std::endl; }
};

class PaymentFactory {
private:
    using CreatorFunc = std::function<std::unique_ptr<paymentProcessor>()>;
    std::unordered_map<std::string, CreatorFunc> registry;
public:
    PaymentFactory() {
        // registrar todos os tipos possíveis
        registry["PIX"] = []() { return std::make_unique<Product<PIX>>(); };
        registry["MERCADOPAGO"] = []() { return std::make_unique<Product<MERCADOPAGO>>(); };
        registry["PAGARME"] = []() { return std::make_unique<Product<PAGARME>>(); };
        registry["GENERIC"] = []() { return std::make_unique<Product<void>>(); };
    }

    std::unique_ptr<paymentProcessor> createProduct(const std::string& type) {
        if (registry.find(type) != registry.end())
            return registry[type]();
        return nullptr;
    }
};


int main() {

    PaymentFactory factory;

    std::cout << "===========================" << std::endl;
    std::cout << "Creating a pix product:" << std::endl;
    
    auto p1 = factory.createProduct("PIX");
    p1->operation();
    p1->getPaymentInfo();

    std::cout << "===========================" << std::endl;
    std::cout << "Creating a mercadopago product:" << std::endl;

    auto p2 = factory.createProduct("MERCADOPAGO");
    p2->operation();
    p2->getPaymentInfo();
    
    std::cout << "===========================" << std::endl;
    std::cout << "Creating a pagarme product:" << std::endl;

    auto p3 = factory.createProduct("PAGARME");
    p3->operation();
    p3->getPaymentInfo();

    std::cout << "===========================" << std::endl;
    std::cout << "Creating a generic product:" << std::endl;
    
    auto p4 = factory.createProduct("GENERIC");
    p4->operation();
    p4->getPaymentInfo();

    std::cout << "===========================" << std::endl;
    std::cout << "dealocate memory:" << std::endl;

    return 0;
}