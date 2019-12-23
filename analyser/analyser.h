#pragma once

#include "error/error.h"
#include "instruction/instruction.h"
#include "tokenizer/token.h"
#include "symbletable/symbletable.h"
#include <vector>
#include <optional>
#include <utility>
#include <map>
#include <cstdint>
#include <cstddef> // for std::size_t

namespace miniplc0 {

	class Analyser final {
	private:
		using uint64_t = std::uint64_t;
		using int64_t = std::int64_t;
		using uint32_t = std::uint32_t;
		using int32_t = std::int32_t;
	public:
		Analyser(std::vector<Token> v)
			: _tokens(std::move(v)), _offset(0), _instructions({}), _current_pos(0, 0),
              _symbleTable(),_all_index(0), _nextTokenIndex(0), _stringTable({}), _functions({}){}
		Analyser(Analyser&&) = delete;
		Analyser(const Analyser&) = delete;
		Analyser& operator=(Analyser) = delete;

		// 唯一接口
		std::pair<std::vector<Instruction>, std::optional<CompilationError>> Analyse();
	private:

		// Token 缓冲区相关操作
		// 返回下一个 token
		std::optional<Token> nextToken();
		// 回退一个 token
		void unreadToken();

		// 下面是符号表相关操作


	private:
		std::vector<Token> _tokens;
		std::size_t _offset;
		std::vector<Instruction> _instructions;
		std::pair<uint64_t, uint64_t> _current_pos;
		//todo:符号表
        SymbleTable _symbleTable;
        int32_t _all_index;
        std::vector<std::string> _stringTable;
        std::vector<function> _functions;
		// 下一个 token 在栈的偏移
		int32_t _nextTokenIndex;




        //C0
        std::optional<CompilationError> C0_program();

        std::optional<CompilationError> function_definition();

        std::optional<CompilationError> expression(bool isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> cast_expression(bool *isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> multiplicative_expression(bool *isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> additive_expression(bool *isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> primary_expression(bool *isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> variable_declaration(SymbleTable *symbleTable);

        std::optional<CompilationError> init_declarator_list(bool isCONST, bool isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> init_declarator(bool isCONST, bool isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> compound_statement(SymbleTable *symbleTable);

        std::optional<CompilationError> parameter_declaration(SymbleTable *symbleTable);

        std::optional<CompilationError> statement(SymbleTable *symbleTable);

        std::optional<CompilationError> assignment_expression(SymbleTable *symbleTable);

        std::optional<CompilationError> function_call(SymbleTable *symbleTable);

        std::optional<CompilationError> scan_statement(SymbleTable *symbleTable);

        void addString(const std::string& string);

        int32_t stringIndex(const std::string& string);

        std::optional<CompilationError> print_statement(SymbleTable *symbleTable);

        std::optional<CompilationError> printable(SymbleTable *symbleTable);

        std::optional<CompilationError> print_expression(bool *isINT, SymbleTable *symbleTable);
    };
}