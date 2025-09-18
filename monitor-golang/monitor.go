package main

import (
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"io"
	"io/fs"
	"os"
	"path/filepath"
	"strings"
	"time"
)

var inputDir = "/workspaces/design-patterns/monitor-golang/input"
var backupDir = "/workspaces/design-patterns/monitor-golang/output"

// calcular hash SHA-256
func calcularHash(caminho string) (string, error) {
	file, err := os.Open(caminho)
	if err != nil {
		return "", err
	}
	defer file.Close()

	hash := sha256.New()
	if _, err := io.Copy(hash, file); err != nil {
		return "", err
	}
	return hex.EncodeToString(hash.Sum(nil)), nil
}

// restaurar arquivo por hash
func restaurarPorHash(nomeBase, hashParcial string) {
	found := false

	err := filepath.WalkDir(backupDir, func(path string, d fs.DirEntry, err error) error {
		if err != nil {
			return err
		}
		if !d.IsDir() {
			nome := d.Name()
			if strings.HasPrefix(nome, nomeBase+"_") && strings.HasPrefix(nome[len(nomeBase)+1:], hashParcial) {
				destino := filepath.Join(inputDir, nomeBase)
				input, _ := os.Open(path)
				defer input.Close()
				output, _ := os.Create(destino)
				defer output.Close()
				io.Copy(output, input)
				fmt.Printf("✅ Restaurado %s a partir do hash %s\n", nomeBase, hashParcial)
				found = true
				return io.EOF // stop walk
			}
		}
		return nil
	})

	if err != nil && err != io.EOF {
		fmt.Println("Erro ao restaurar:", err)
		return
	}

	if !found {
		fmt.Printf("❌ Versão não encontrada para hash: %s\n", hashParcial)
	}
}

// listar hashes disponíveis
func listarHashes(nomeBase string) {
	hashes := []string{}

	filepath.WalkDir(backupDir, func(path string, d fs.DirEntry, err error) error {
		if err != nil {
			return err
		}
		if !d.IsDir() {
			nome := d.Name()
			if strings.HasPrefix(nome, nomeBase+"_") {
				hash := nome[len(nomeBase)+1:]
				hashes = append(hashes, hash)
			}
		}
		return nil
	})

	if len(hashes) == 0 {
		fmt.Printf("Nenhuma versão encontrada para %s\n", nomeBase)
	} else {
		fmt.Printf("Hashes disponíveis para %s:\n", nomeBase)
		for _, h := range hashes {
			fmt.Printf(" - %s\n", h)
		}
	}
}

// mostrar ajuda
func mostrarHelp() {
	fmt.Println("Uso: monitor_app [OPÇÃO] [ARGUMENTOS]")
	fmt.Println("\nSem argumentos                               : Inicia o monitoramento da pasta de input")
	fmt.Println("--list <arquivo>                             : Lista todas as versões (hashes) disponíveis para o arquivo")
	fmt.Println("--revert <arquivo> <hash>                    : Restaura a versão do arquivo correspondente ao hash (parcial ou completo)")
	fmt.Println("--help                                       : Ajuda")
	fmt.Println("\nExemplos:")
	fmt.Println("  ./monitor_app                              : inicia monitoramento")
	fmt.Println("  ./monitor_app --list arquivo.txt           : lista versões do arquivo")
	fmt.Println("  ./monitor_app --revert arquivo.txt 3a7b    : restaura versão do arquivo")
}

func main() {
	if _, err := os.Stat(inputDir); os.IsNotExist(err) {
		fmt.Println("Diretório inválido:", inputDir)
		return
	}
	os.MkdirAll(backupDir, os.ModePerm)

	args := os.Args

	// --help
	if len(args) == 2 && args[1] == "--help" {
		mostrarHelp()
		return
	}

	// --revert
	if len(args) == 4 && args[1] == "--revert" {
		restaurarPorHash(args[2], args[3])
		return
	}

	// --list
	if len(args) == 3 && args[1] == "--list" {
		listarHashes(args[2])
		return
	}

	// argumento inválido
	if len(args) > 1 {
		fmt.Println("❌ Parâmetro inválido ou incompleto.")
		mostrarHelp()
		return
	}

	// monitoramento
	arquivosAnteriores := make(map[string]time.Time)
	fmt.Printf("📡 Monitorando %s e salvando versões em %s\n", inputDir, backupDir)

	for {
		filepath.WalkDir(inputDir, func(path string, d fs.DirEntry, err error) error {
			if err != nil || d.IsDir() {
				return nil
			}
			nome := d.Name()
			info, _ := os.Stat(path)
			modTime := info.ModTime()

			if t, ok := arquivosAnteriores[nome]; !ok || !modTime.Equal(t) {
				hash, err := calcularHash(path)
				if err != nil {
					fmt.Println("Erro ao calcular hash:", err)
					return nil
				}
				destino := filepath.Join(backupDir, nome+"_"+hash)
				input, _ := os.Open(path)
				defer input.Close()
				output, _ := os.Create(destino)
				defer output.Close()
				io.Copy(output, input)
				fmt.Println("💾 Nova versão salva:", destino)
				arquivosAnteriores[nome] = modTime
			}

			return nil
		})

		time.Sleep(2 * time.Second)
	}
}
