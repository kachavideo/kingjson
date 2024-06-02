#include <iostream>
#include <string>
#include <time.h>
#include "../CKingJson.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "The argument is wrong!\n";
        return 0;
    }  
    CKingJson kingJson;
    kingJson.OpenFileA(argv[1], 0);
    std::cout << "It had opened file.\n";
    return 0;
}
