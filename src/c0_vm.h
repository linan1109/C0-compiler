#include "./vm.h"
#include "./file.h"
#include "./exception.h"
#include "print.hpp"
#include "argparse.hpp"

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <exception>
class c0_vm{
    public:c0_vm(){}

        void assemble_text(std::ifstream *in, std::ofstream *out) {
            try {
                File f = File::parse_file_text(*in);
                // f.output_text(std::cout);
                f.output_binary(*out);
            }
            catch (const std::exception &e) {
                println(std::cerr, e.what());
            }
        }



};