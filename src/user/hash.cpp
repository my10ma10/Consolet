#include <string>
#include <numeric>

#define LANG_CHARS 26


std::string hash(std::string str) {
    unsigned int shift = std::accumulate(str.begin(), str.end(), 0) % LANG_CHARS;
    
    for (char& symbol : str) {
        char base = std::isupper(symbol) ? 'A' : 'a';
        auto num = symbol - base;

        symbol = (num + shift + LANG_CHARS) % LANG_CHARS + base;
    }
    // std::cout << "new password : " << str << std::endl;
    return str;
}