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
    public:
        const std::vector<std::string> &getStringTable() const;

        const std::vector<function> &getFunctions() const;

    private:
		using uint64_t = std::uint64_t;
		using int64_t = std::int64_t;
		using uint32_t = std::uint32_t;
		using int32_t = std::int32_t;
	public:
		Analyser(std::vector<Token> v)
			: _tokens(std::move(v)), _offset(0), _instructions({}), _current_pos(0, 0),
              _symbleTable(),_all_index(0), _stringTable({}), _functions({}), _nextTokenIndex(0){}
		Analyser(Analyser&&) = delete;
		Analyser(const Analyser&) = delete;
		Analyser& operator=(Analyser) = delete;

		// 唯一接口
        std::pair<std::pair<std::vector<Instruction>, std::vector<std::vector<Instruction>> >, std::optional<CompilationError>> Analyse();
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
		int32_t __start_count = 0;

		std::vector< std::pair<std::string, std::vector<int32_t > > > _while_break_count;




        //C0
        std::optional<CompilationError> C0_program();

        std::optional<CompilationError> expression(const std::string &funname, bool isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> cast_expression(const std::string &funname, bool *isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> multiplicative_expression(const std::string &funname, bool *isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> additive_expression(const std::string &funname, bool *isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> primary_expression(const std::string &funname, bool *isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> variable_declaration(const std::string &funname, SymbleTable *symbleTable);

        std::optional<CompilationError> init_declarator_list(const std::string &funname, bool isCONST, bool isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> init_declarator(const std::string &funname, bool isCONST, bool isINT, SymbleTable *symbleTable);

        std::optional<CompilationError> assignment_expression(const std::string &funname, SymbleTable *symbleTable);

        std::optional<CompilationError> function_call(const std::string &funname, SymbleTable *symbleTable);

        std::optional<CompilationError> scan_statement(const std::string &funname, SymbleTable *symbleTable);

        void addString(const std::string& string);

        int32_t stringIndex(const std::string& string);

        std::optional<CompilationError> print_statement(const std::string &funname, SymbleTable *symbleTable);

        std::optional<CompilationError> printable(const std::string &funname, SymbleTable *symbleTable);

        std::optional<CompilationError> print_expression(const std::string &funname, bool *isINT, SymbleTable *symbleTable);

        int32_t functionIndex(const std::string &string);

        bool
        addParamByFunName(const std::string &param, const int32_t kind, const int32_t type, const std::string &name);

        std::optional<CompilationError> parameter_declaration(const std::string &funname, SymbleTable *symbleTable);

        std::optional<CompilationError> function_definition();

        std::string newAUuid();

        bool addInstructionByFunName(Operation operation, int32_t x, int32_t y, const std::string &name);

        bool addFun(const std::string &name, const int32_t type, const int32_t name_index, const int32_t level);

        int32_t functionType(const std::string &string);


        std::optional<CompilationError> check_main();

        bool changeXByCountAndFunName(int32_t count, int32_t x, const std::string &name);

        std::optional<CompilationError>
        compound_statement(const std::string & whilename, const std::string & funname,  SymbleTable *symbleTable, int32_t funtype);

        std::optional<CompilationError>
        fun_compound_statement(const std::string &funname, int32_t &type, SymbleTable *symbleTable);

        std::optional<CompilationError> statement(const std::string & whilename, const std::string & funname,  SymbleTable *symbleTable, int32_t returntype);

        std::optional<CompilationError>
        condition_statement(const std::string & whilename, const std::string &funname, SymbleTable *symbleTable, int32_t returntype);

        std::optional<CompilationError>
        while_statement(const std::string &funname, SymbleTable *symbleTable, int32_t returntype);

        std::optional<CompilationError>
        condition(const std::string &funname,  SymbleTable *symbleTable, int32_t *index);

        int32_t getCountByFunName(const std::string &name);

        std::string addWhile();

        void addWhileBreak( const std::string& name, const int32_t count);

        std::vector<int32_t> getWhileBreaksByName(const std::string &name);
    };
}