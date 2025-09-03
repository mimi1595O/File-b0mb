#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: antidote <directory>\n";
        return 1;
    }

    fs::path target = argv[1];

    if (!fs::exists(target)) {
        std::cerr << "Error: target does not exist.\n";
        return 1;
    }

    std::error_code ec;
    auto count = fs::remove_all(target, ec);

    if (ec) {
        std::cerr << "Some errors occurred while deleting: " << ec.message() << "\n";
    } else {
        std::cout << "Successfully deleted " << count << " files/directories from " 
                  << target << "\n";
    }

    return 0;
}
