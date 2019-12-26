#include "argparse.hpp"
#include "fmt/core.h"

#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"
#include "fmts.hpp"

#include <iostream>
#include <fstream>
#include <src/c0_vm.h>

std::vector<LNC0::Token> _tokenize(std::istream& input) {
	LNC0::Tokenizer tkz(input);
	auto p = tkz.AllTokens();
	if (p.second.has_value()) {
		fmt::print(stderr, "Tokenization error: {}\n", p.second.value());
		exit(2);
	}
	return p.first;
}

void Tokenize(std::istream& input, std::ostream& output) {
	auto v = _tokenize(input);
	for (auto& it : v)
		output << fmt::format("{}\n", it);
	return;
}

void Analyse(std::istream& input, std::ostream& output){
	auto tks = _tokenize(input);
	LNC0::Analyser analyser(tks);
	auto p = analyser.Analyse();
	if (p.second.has_value()) {
		fmt::print(stderr, "Syntactic analysis error: {}\n", p.second.value());
		exit(2);
	}
	output <<".constants:\n";
	auto stringtable = analyser.getStringTable();
	int co = 0;
	for(auto &s :stringtable){
	    output <<co++<<" S \""<<s<<"\"\n";
	}


	auto v = p.first.first;
	for (auto& it : v)
		output << fmt::format("{}\n", it);

    output <<".functions:\n";
	auto funs = analyser.getFunctions();
    co = 0;
    for(auto &s :funs){
        output <<co++<<" "<<s.getNameIndex()<<" "<<s.getParams().size()<<" "<<s.getLevel()<<"\n";
    }

    std::vector<std::vector<LNC0::Instruction> >  y = p.first.second;
	for(auto &x : y)
        for (auto& it : x)
            output << fmt::format("{}\n", it);

	return;
}

int main(int argc, char** argv) {
	argparse::ArgumentParser program("LNC0");
	program.add_argument("input")
		.help("speicify the file to be compiled.");
	program.add_argument("-c")
		.default_value(false)
		.implicit_value(true)
		.help("perform Binary target file for the input file.");
	program.add_argument("-s")
		.default_value(false)
		.implicit_value(true)
		.help("perform Text assembly file for the input file.");
	program.add_argument("-o", "--output")
		.required()
		.default_value(std::string("out"))
		.help("specify the output file.");

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err) {
		fmt::print(stderr, "{}\n\n", err.what());
		program.print_help();
		exit(2);
	}

	auto input_file = program.get<std::string>("input");
	auto output_file = program.get<std::string>("--output");
	std::istream* input;
	std::ostream* output;
	std::ifstream inf;
	std::ofstream outf;
	if (input_file != "-") {
		inf.open(input_file, std::ios::in);
		if (!inf) {
			fmt::print(stderr, "Fail to open {} for reading.\n", input_file);
			exit(2);
		}
		input = &inf;
	}
	else
		input = &std::cin;
	if (output_file != "-") {
		outf.open(output_file, std::ios::out | std::ios::trunc);
		if (!outf) {
			fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
			exit(2);
		}
		output = &outf;
	}
	else
		output = &std::cout;
	if (program["-c"] == true && program["-s"] == true) {
		fmt::print(stderr, "You can only perform tokenization or syntactic analysis at one time.");
		exit(2);
	}
	if (program["-c"] == true) {
		//Tokenize(*input, *output);
        c0_vm c0Vm;
        std::ifstream newinf;
        std::ofstream newoutf;


        newoutf.open("middle.txt", std::ios::binary | std::ios::out | std::ios::trunc);
        if (!newoutf) {
            fmt::print(stderr, "Fail to open {} for writing.\n", "middle.s");
            exit(2);
        }
        std::ostream* newoutput = &newoutf;
        Analyse(*input, *newoutput);
        newoutf.close();
        newinf.open("middle.txt", std::ios::binary | std::ios::in);
        if (!newinf) {
            fmt::print(stderr, "Fail to open {} for reading.\n", "middle.s");
            exit(2);
        }
        c0Vm.assemble_text(&newinf, &outf);
        outf.close();
	}
	else if (program["-s"] == true) {
        //Tokenize(*input, *output);
		Analyse(*input, *output);
	}
	else {
		fmt::print(stderr, "You must choose tokenization or syntactic analysis.");
		exit(2);
	}
	return 0;
}