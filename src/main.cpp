#include "Parser.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Required file argument: ./fit_reader <filename>" << std::endl;
        return 1;
    }

    std::ifstream source(argv[1], std::ios::in | std::ios::binary);
    if (!source.is_open()) {
        std::cerr << "Error with file opening!" << std::endl;
        return 1;
    }

    Parser parser;

    do
    {
        Record new_record;

        new_record.ParceData(&source);
        if (!source.good()) {
            break;
        }

        parser.opcode_to_record[new_record.opcode].push_back(std::move(new_record));
    } while (source.good());
    
    parser.PrintData();

    return 0;
}
