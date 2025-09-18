#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

namespace fs = std::filesystem;

// calcular hash SHA-256
std::string calcular_hash(const fs::path &arquivo) {
    std::ifstream in(arquivo, std::ios::binary);
    if (!in) return "";

    std::ostringstream oss;
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    const size_t buffer_size = 4096;
    char buffer[buffer_size];

    while (in.read(buffer, buffer_size) || in.gcount() > 0) {
        SHA256_Update(&sha256, buffer, in.gcount());
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return oss.str();
}

// restaurar arquivo por hash
void restaurar_por_hash(const fs::path &backup_dir, const fs::path &input_dir,
                        const std::string &nome_base, const std::string &hash_parcial) {
    for (auto &entry : fs::directory_iterator(backup_dir)) {
        if (entry.is_regular_file()) {
            std::string nome = entry.path().filename().string();
            if (nome.find(nome_base + "_") == 0 &&
                nome.substr(nome_base.size() + 1).find(hash_parcial) == 0) {
                fs::path destino = input_dir / nome_base;
                fs::copy_file(entry.path(), destino, fs::copy_options::overwrite_existing);
                std::cout << "✅ Restaurado " << nome_base << " a partir do hash " << hash_parcial << std::endl;
                return;
            }
        }
    }
    std::cerr << "❌ Versão não encontrada para hash: " << hash_parcial << std::endl;
}

// listar hashes disponíveis
void listar_hashes(const fs::path &backup_dir, const std::string &nome_base) {
    std::vector<std::string> hashes;

    for (auto &entry : fs::directory_iterator(backup_dir)) {
        if (entry.is_regular_file()) {
            std::string nome = entry.path().filename().string();
            if (nome.find(nome_base + "_") == 0) {
                std::string hash = nome.substr(nome_base.size() + 1);
                hashes.push_back(hash);
            }
        }
    }

    if (hashes.empty()) {
        std::cout << "Nenhuma versão encontrada para " << nome_base << std::endl;
    } else {
        std::cout << "Hashes disponíveis para " << nome_base << ":\n";
        for (auto &h : hashes) {
            std::cout << " - " << h << "\n";
        }
    }
}

// mostrar ajuda
void mostrar_help() {
    std::cout << "Uso: monitor_app [OPÇÃO] [ARGUMENTOS]\n\n";
    std::cout << "Sem argumentos                               : Inicia o monitoramento da pasta de input\n";
    std::cout << "--list <arquivo>                             : Lista todas as versões (hashes) disponíveis para o arquivo\n";
    std::cout << "--revert <arquivo> <hash>                    : Restaura a versão do arquivo correspondente ao hash (parcial ou completo)\n";
    std::cout << "--help                                       : Ajuda\n\n";
    std::cout << "Exemplos:\n";
    std::cout << "  ./monitor_app                              : inicia monitoramento\n";
    std::cout << "  ./monitor_app --list arquivo.txt           : lista versões do arquivo\n";
    std::cout << "  ./monitor_app --revert arquivo.txt 3a7b    : restaura versão do arquivo\n";
}

int main(int argc, char *argv[]) {
    fs::path dir = "/workspaces/design-patterns/monitor-cpp/input";
    fs::path backup_dir = "/workspaces/design-patterns/monitor-cpp/output";

    if (!fs::exists(dir) || !fs::is_directory(dir)) {
        std::cerr << "Diretório inválido: " << dir << std::endl;
        return 1;
    }
    if (!fs::exists(backup_dir)) {
        fs::create_directories(backup_dir);
    }

    // --help
    if (argc == 2 && std::string(argv[1]) == "--help") {
        mostrar_help();
        return 0;
    }

    // modo revert
    if (argc == 4 && std::string(argv[1]) == "--revert") {
        std::string arquivo = argv[2];
        std::string hash = argv[3];
        restaurar_por_hash(backup_dir, dir, arquivo, hash);
        return 0;
    }

    // modo list
    if (argc == 3 && std::string(argv[1]) == "--list") {
        std::string arquivo = argv[2];
        listar_hashes(backup_dir, arquivo);
        return 0;
    }

    // qualquer outro argumento inválido
    if (argc > 1) {
        std::cerr << "❌ Parâmetro inválido ou incompleto.\n\n";
        mostrar_help();
        return 1;
    }

    // monitoramento
    std::unordered_map<std::string, fs::file_time_type> arquivos_anteriores;
    std::cout << "📡 Monitorando " << dir << " e salvando versões em " << backup_dir << std::endl;

    while (true) {
        for (auto &entry : fs::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                std::string nome = entry.path().filename().string();
                auto mod_time = entry.last_write_time();

                if (!arquivos_anteriores.count(nome) || arquivos_anteriores[nome] != mod_time) {
                    std::string hash = calcular_hash(entry.path());
                    fs::path destino = backup_dir / (nome + "_" + hash);

                    try {
                        fs::copy_file(entry.path(), destino, fs::copy_options::overwrite_existing);
                        std::cout << "💾 Nova versão salva: " << destino << std::endl;
                        arquivos_anteriores[nome] = mod_time;
                    } catch (const std::exception &e) {
                        std::cerr << "Erro salvando versão: " << e.what() << std::endl;
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}

