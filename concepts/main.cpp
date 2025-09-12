#include <concepts>
#include <iostream>
#include <type_traits>

// Define um concept que aceita apenas tipos inteiros
template<typename T>
concept Inteiro = std::integral<T>;

// Função que só aceita tipos inteiros
template<Inteiro T>
T dobro(T valor) {
    return valor * 2;
}

int main() {
    std::cout << dobro(10) << '\n';      // ✅ ok, int é inteiro
    std::cout << dobro(42u) << '\n';     // ✅ ok, unsigned int é inteiro
    // std::cout << dobro(3.14) << '\n';    // ❌ erro de compilação: double não é inteiro
}

/*
C++20 introduziu os "concepts", uma poderosa ferramenta para definir restrições em templates. Concepts permitem especificar, de forma clara e segura, quais operações e propriedades um tipo deve possuir para ser aceito por um template. Isso facilita a escrita de código genérico mais legível, seguro e com mensagens de erro mais amigáveis. Com concepts, você pode garantir que funções e classes só sejam instanciadas com tipos adequados, evitando bugs difíceis de encontrar.

    - Legibilidade: O código fica mais fácil de entender, pois as restrições dos tipos são declaradas diretamente.
    - Segurança: Erros de uso de tipos inadequados são detectados em tempo de compilação, evitando bugs difíceis de rastrear.
    - Mensagens de erro melhores: O compilador fornece mensagens mais claras quando um tipo não atende aos requisitos do concept.
    - Facilidade de manutenção: Fica mais simples modificar ou ampliar templates, pois os requisitos estão centralizados nos concepts.
*/