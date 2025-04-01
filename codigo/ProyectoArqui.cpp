#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <fstream>
#include <iomanip>
#include <sstream>

class Cache {
public:
    int size;
    int associativity;
    int lineSize;
    std::map<int, int> data;
    std::list<int> lruList;

    Cache(int size, int associativity, int lineSize) 
        : size(size), associativity(associativity), lineSize(lineSize) {}

    bool access(int address) {
        int tag = address / lineSize;
        if (data.find(tag) != data.end()) {
            lruList.remove(tag);
            lruList.push_front(tag);
            return true;
        } else {
            if (data.size() >= size / lineSize) {
                int lruTag = lruList.back();
                lruList.pop_back();
                data.erase(lruTag);
            }
            data[tag] = address;
            lruList.push_front(tag);
            return false;
        }
    }

    bool isFull() const {
        return data.size() >= size / lineSize;
    }

    std::string getCacheState() const {
        std::stringstream ss;
        int totalLines = size / lineSize;
        ss << "Cache (" << totalLines << " lineas):\n";
        for (const auto& entry : data) {
            ss << "L" << entry.first << ": Tag " << entry.first << " (Addr " << entry.second << ")\n";
        }
        for (int i = 0; i < totalLines - data.size(); i++) {
            ss << "L" << i + data.size() << ": Vacia\n";
        }
        return ss.str();
    }
};

class Memory {
public:
    std::vector<int> data;

    Memory(int size) : data(size, 0) {}

    int read(int address) {
        return data[address];
    }
};

void printAccessAndCache(const std::string& accessInfo, const std::string& l1State, 
                        const std::string& l2State, const std::string& l3State) {
    std::vector<std::vector<std::string>> sections(4);
    std::stringstream ss;
    
    ss.str(accessInfo);
    std::string line;
    while (std::getline(ss, line)) sections[0].push_back(line);
    ss.clear();
    
    ss.str(l1State);
    while (std::getline(ss, line)) sections[1].push_back(line);
    ss.clear();
    
    ss.str(l2State);
    while (std::getline(ss, line)) sections[2].push_back(line);
    ss.clear();
    
    ss.str(l3State);
    while (std::getline(ss, line)) sections[3].push_back(line);
    
    size_t maxLines = 0;
    for (const auto& section : sections) {
        if (section.size() > maxLines) maxLines = section.size();
    }

    std::cout << std::left;
    std::cout << std::setw(40) << "Acceso y Resultados";
    std::cout << std::setw(30) << "Cache L1";
    std::cout << std::setw(30) << "Cache L2";
    std::cout << std::setw(30) << "Cache L3";
    std::cout << "\n" << std::string(130, '-') << "\n";
    
    for (size_t i = 0; i < maxLines; ++i) {
        std::cout << std::setw(40) << (i < sections[0].size() ? sections[0][i] : "");
        std::cout << std::setw(30) << (i < sections[1].size() ? sections[1][i] : "");
        std::cout << std::setw(30) << (i < sections[2].size() ? sections[2][i] : "");
        std::cout << std::setw(30) << (i < sections[3].size() ? sections[3][i] : "");
        std::cout << "\n";
    }
    std::cout << std::string(130, '=') << "\n\n";
}

int main() {
    Cache l1(32, 2, 16);
    Cache l2(64, 4, 16);
    Cache l3(128, 8, 16);
    Memory mainMemory(1024);

    std::ifstream inputFile("entrada.txt");
    if (inputFile.is_open()) {
        std::vector<int> addresses;
        int address;
        while (inputFile >> address) {
            addresses.push_back(address);
        }
        inputFile.close();

        for (int addr : addresses) {
            std::stringstream accessInfo;
            accessInfo << "Acceso a direccion: " << addr << "\n";

            if (!l1.access(addr)) {
                accessInfo << "Fallo en L1\n";
                if (!l2.access(addr)) {
                    accessInfo << "Fallo en L2\n";
                    if (!l3.access(addr)) {
                        accessInfo << "Fallo en L3\n";
                        mainMemory.read(addr);
                        accessInfo << "Acceso a memoria principal\n";
                        l3.access(addr);
                        l2.access(addr);
                        l1.access(addr);
                    } else {
                        accessInfo << "Acierto en L3\n";
                        l2.access(addr);
                        l1.access(addr);
                    }
                } else {
                    accessInfo << "Acierto en L2\n";
                    l1.access(addr);
                }
            } else {
                accessInfo << "Acierto en L1\n";
            }

            printAccessAndCache(accessInfo.str(), l1.getCacheState(), l2.getCacheState(), l3.getCacheState());
        }
    } else {
        std::cout << "Archivo 'entrada.txt' no encontrado. Puede ingresar direcciones manualmente.\n";
    }

    while (true) {
        int address;
        std::cout << "\nIngresa una direccion de memoria (o -1 para salir): ";
        std::cin >> address;

        if (address == -1) {
            break;
        }

        std::stringstream accessInfo;
        accessInfo << "Acceso a direccion: " << address << "\n";

        if (!l1.access(address)) {
            accessInfo << "Fallo en L1\n";
            if (!l2.access(address)) {
                accessInfo << "Fallo en L2\n";
                if (!l3.access(address)) {
                    accessInfo << "Fallo en L3\n";
                    mainMemory.read(address);
                    accessInfo << "Acceso a memoria principal\n";
                    l3.access(address);
                    l2.access(address);
                    l1.access(address);
                } else {
                    accessInfo << "Acierto en L3\n";
                    l2.access(address);
                    l1.access(address);
                }
            } else {
                accessInfo << "Acierto en L2\n";
                l1.access(address);
            }
        } else {
            accessInfo << "Acierto en L1\n";
        }

        printAccessAndCache(accessInfo.str(), l1.getCacheState(), l2.getCacheState(), l3.getCacheState());
    }

    return 0;
}