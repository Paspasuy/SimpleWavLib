#include <fstream>
#include <iostream>

std::string read_str(std::ifstream& inp, size_t size) {
    std::string res;
    for (size_t it = 0; it < size; ++it) {
        char g = inp.get();
        res += g;
    }
    return res;
}

size_t read_num(std::ifstream& inp, size_t size) {
    size_t res = 0;
    inp.read(reinterpret_cast<char*>(&res), size);
    return res;
}

void check_str(std::ifstream& inp, const std::string& str) {
    std::string tmp;
    if ((tmp = read_str(inp, str.size())) != str) {
        std::cerr << str.size() << '\n';
        for (size_t kek = 0; kek < str.size(); ++kek) {
            std::cerr << "got: " << int(tmp[kek]) << std::endl;
        }
        std::cerr << "Wrong file format" << std::endl;
        throw std::runtime_error("Wrong file format");
    }
}

void write_num(std::ofstream& out, size_t num, size_t size) {
    for (size_t it = 0; it < size; ++it) {
        out.put(num & 0xff);
        num >>= 8;
    }
}


