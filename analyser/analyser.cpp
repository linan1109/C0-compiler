#include "analyser.h"

#include <climits>
#include <sstream>
#include <objbase.h>


namespace LNC0 {

    std::pair<std::pair<std::vector<Instruction>, std::vector<std::vector<Instruction>> >, std::optional<CompilationError>> Analyser::Analyse() {
        auto err = C0_program();
        //std::printf("***********************END************************** \n");
        if (err.has_value())
            return std::make_pair(std::make_pair(std::vector<Instruction>(), std::vector<std::vector<Instruction>>()), err);
        else{
            std::vector<std::vector<Instruction>> vcc;
            for(auto i: _functions)
                vcc.push_back(i.getInstructions());
            return std::make_pair(std::make_pair(_instructions, vcc), std::optional<CompilationError>());
        }

    }


    std::optional<CompilationError> Analyser::check_main() {
        if(functionIndex("main") < 0)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedMain);
        return{};
    }
    //<C0-program> ::=
    //        {<变量_声明>}{<函数_定义>}
    //<C0_program> ::=
    //    {<variable_declaration>}{<function_definition>}
    std::optional<CompilationError> Analyser::C0_program() {
        
        addInstructionByFunName(Operation::_start, 0, 0, "");
        addInstructionByFunName(Operation::nop, 0, 0, "");
        while(true){
            auto ed = nextToken();
            if(!ed.has_value()) break;
            if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD){
                if(ed.value().GetValueString() == "const"){
                    unreadToken();
                    auto err = variable_declaration("",&_symbleTable);
                    if (err.has_value())
                        return err;
                }
                else if (ed.value().GetValueString() == "void"){
                    unreadToken();
                    auto err = function_definition();
                    if (err.has_value())
                        return err;
                }else if(ed.value().GetValueString() == "char"){
                    ed = nextToken();
                    if(!ed.has_value() || !(ed.value().GetType() == TokenType::IDENTIFIER))
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);

                    ed = nextToken();
                    if(ed.has_value() && (ed.value().GetType() == TokenType::EQUAL_SIGN
                    || ed.value().GetType() == TokenType::COMMA_SIGN
                    || ed.value().GetType() == TokenType::SEMICOLON)){
                        unreadToken();
                        unreadToken();
                        unreadToken();
                        auto err = variable_declaration("", &_symbleTable);
                        if (err.has_value())
                            return err;
                    }else if(ed.has_value() && ed.value().GetType() == TokenType::LEFT_BRACKET){
                        unreadToken();
                        unreadToken();
                        unreadToken();
                        auto err = function_definition();
                        if(err.has_value())
                            return err;
                    }


                }else if (ed.value().GetValueString() == "int"){
                    ed = nextToken();
                    if(!ed.has_value() || !(ed.value().GetType() == TokenType::IDENTIFIER))
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);

                    ed = nextToken();
                    if(ed.has_value() && (ed.value().GetType() == TokenType::EQUAL_SIGN
                    || ed.value().GetType() == TokenType::COMMA_SIGN
                    || ed.value().GetType() == TokenType::SEMICOLON)){
                        unreadToken();
                        unreadToken();
                        unreadToken();
                        auto err = variable_declaration("",&_symbleTable);
                        if (err.has_value())
                            return err;
                    }else if(ed.has_value() && ed.value().GetType() == TokenType::LEFT_BRACKET){
                        unreadToken();
                        unreadToken();
                        unreadToken();
                        auto err = function_definition();
                        if(err.has_value())
                            return err;
                    }
                }else
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoType_specifier);
            }else
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoType_specifier);
        }
        auto err = check_main();
        if(err.has_value())
            return err;

        return {};
    }


    //<变量_声明> ::=
    //    [<const_修饰符>]<类型_说明符号><声明_语句_列表>';'
    //<variable_declaration> ::=
    //    [<const_qualifier>]<type_specifier><init_declarator_list>';'
    std::optional<CompilationError> Analyser::variable_declaration(const std::string &funname, SymbleTable *symbleTable) {
        //[<const_qualifier>]
        bool isCONST = true;
        auto ed = nextToken();
        if (!ed.has_value() || ed.value().GetType() != TokenType::RESERVED_WORD || ed.value().GetValueString() != "const"){
            isCONST = false;
            unreadToken();
        }
        //<type_specifier>
        ed = nextToken();
        bool isINT = true;
        if (ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "int"){

        }
        else if (ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "char"){
            isINT = false;
        }else
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoType_specifier);
        auto err = init_declarator_list(funname, isCONST, isINT, symbleTable);
        if (err.has_value())
            return err;

        ed = nextToken();
        if (!ed.has_value() || ed.value().GetType() != TokenType::SEMICOLON){
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        }
        return {};
    }
    //<声明_语句_列表> ::=
    //    <声明_语句>{','<声明_语句>}
    //<init_declarator_list> ::=
    //    <init_declarator>{','<init_declarator>}
    std::optional<CompilationError> Analyser::init_declarator_list(const std::string &funname, bool isCONST, bool isINT, SymbleTable *symbleTable) {
        auto err = init_declarator(funname, isCONST, isINT, symbleTable);
        if (err.has_value())
            return err;
        auto ed = nextToken();
        while (ed.has_value() && ed.value().GetType() == TokenType::COMMA_SIGN){
            err = init_declarator(funname, isCONST, isINT, symbleTable);
            if (err.has_value())
                return err;
            ed = nextToken();
        }
        unreadToken();
        return {};
    }
//<声明_语句> ::=
//    <标识符>['='<表达式>]
//<init_declarator> ::=
//    <identifier>['='<expression>]
    std::optional<CompilationError> Analyser::init_declarator(const std::string &funname, bool isCONST, bool isINT, SymbleTable *symbleTable) {
        auto ed = nextToken();
        if (!ed.has_value() || ed.value().GetType() != TokenType::IDENTIFIER ){
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
        }
        auto name = ed.value();
        if(symbleTable->isDeclared(name.GetValueString()))
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
        ed = nextToken();
        
        addInstructionByFunName( Operation::ipush, 0, 0,funname);
        //std::printf("ipush 0\n");
        int32_t type = isINT? 1:2;
        int32_t kind = isCONST? 0:1;

        if(isCONST && !(ed.has_value() && ed.value().GetType() == TokenType::EQUAL_SIGN)){
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrConstantNeedValue);
        }

        if(ed.has_value() && ed.value().GetType() == TokenType::EQUAL_SIGN){
            if(!symbleTable->addSymble(name.GetValueString(),kind,type,1,-1))
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
            int index = symbleTable->index_in_all(name.GetValueString()).first;
            addInstructionByFunName( Operation::loada, 0, index, funname);
            //std::printf("ipush 0\n");
            auto err = expression(funname, isINT,symbleTable);
            if (err.has_value())
                return err;
            //存值
            if(isINT) {
                addInstructionByFunName( Operation::istore, 0, 0,funname);
                //std::printf("istore\n");
            }
            else {
                addInstructionByFunName( Operation::istore, 0, 0,funname);
                //std::printf("istore\n");
            }
        }else {
            if(!symbleTable->addSymble(name.GetValueString(),kind,type,0,-1))
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
            unreadToken();
        }
        return {};
    }




/************************函数部分********************************************/



    //<函数_定义> ::=
    //    <类型_说明符号><标识符> '(' [<参数_声明>{','<参数_声明>}] ')'<作用域_语句>
    //<function-definition> ::=
    //    <type-specifier><identifier>'(' [<parameter_declaration>{','<parameter_declaration>}] ')'<compound-statement>
    std::optional<CompilationError> Analyser::function_definition() {

        int32_t type = 0;
        std::string name;
        auto ed = nextToken();
        if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "void")
            type = 0;
        else if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "int")
            type = 1;
        else if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "char")
            type = 2;
        else return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoType_specifier);
        ed = nextToken();
        if(ed.has_value() && ed.value().GetType() == TokenType::IDENTIFIER){
            name = ed.value().GetValueString();

            if(_symbleTable.getKindByName(name) > 0)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);

            addString(name);
            if(!addFun(name,type,stringIndex(name),1))
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
            _symbleTable.addSymble(name, 2, type, 0, 0);
            int32_t index = functionIndex(name);
            addInstructionByFunName(Operation::_F, index, 0, name);
            //std::printf("function : %s\n",name.c_str());
        }
        else return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);

        SymbleTable *symbleTable = new SymbleTable(&_symbleTable, name);
        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);
        ed = nextToken();
        if(ed.has_value()&& ed.value().GetType() == TokenType::RIGHT_BRACKET){
        }else{
            unreadToken();
            while(true){
                auto err = parameter_declaration(name,symbleTable);
                if (err.has_value())
                    return err;
                ed = nextToken();
                if(!ed.has_value())
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
                if(ed.has_value()&& ed.value().GetType() == TokenType::RIGHT_BRACKET) break;
                if(ed.has_value() && ed.value().GetType() != TokenType::COMMA_SIGN)
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
            }
        }

        //std::printf("get here!!\n");
        auto err = fun_compound_statement(name, type, symbleTable);
        if (err.has_value())
            return err;
        return {};
    }

//<参数_声明> ::=  [<const_修饰符>]<类型_说明符号><标识符>
//<parameter_declaration> ::=  [<const_qualifier>]<type_specifier><identifier>
    std::optional<CompilationError> Analyser::parameter_declaration(const std::string & funname, SymbleTable *symbleTable) {
        auto ed = nextToken();
        int32_t kind = 1,type = 1;
        if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "const") {
            ed = nextToken();
            kind = 0;//常量
        }


        if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "char") {
            ed = nextToken();
            type = 2;
        }else if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "int") {
            ed = nextToken();
        }else return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoType_specifier);


        if(ed.has_value() && ed.value().GetType() == TokenType::IDENTIFIER){
            std::string name = ed.value().GetValueString();
            if(!addParamByFunName(name, kind, type, funname))
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
            symbleTable->addSymble(name,kind,type,1,-1);
        }else return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);

        return {};
    }

//<函数_作用域_语句> ::=  '{' {<变量_声明>} <语句_序列> '}'
//<fun_compound_statement> ::=  '{' {<variable_declaration>} <statement_seq> '}'
    std::optional<CompilationError> Analyser::fun_compound_statement(const std::string & funname,int32_t & type,SymbleTable *symbleTable) {

        auto ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::BIG_LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);

        ed = nextToken();
        while(ed.has_value() && ed.value().GetType() == RESERVED_WORD &&
                (ed.value().GetValueString() == "const" ||
                        ed.value().GetValueString() == "int" ||
                        ed.value().GetValueString() == "char")){
            unreadToken();
            auto err = variable_declaration(funname, symbleTable);
            if(err.has_value())return err;
            ed = nextToken();
        }

        while(ed.has_value() && ed.value().GetType() != BIG_RIGHT_BRACKET ){
            unreadToken();

            auto err = statement("", funname, symbleTable, type);
            if(err.has_value())
                return err;
            ed = nextToken();
        }
        if(!ed.has_value())
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
        if(functionType(funname) == 0)
            addInstructionByFunName( Operation::ret, 0, 0,funname);

        return {};
    }


//<语句_序列> ::=
//	{<语句>}
//<statement_seq> ::=
//	{<statement>}
//<语句> ::=   <作用域_语句> |<条件_语句> |<loop_语句> |<jump_语句> |<print_语句> |<scan_语句> |<赋值_表达式>';' |<函数_call>';' |';'
//<statement> ::=   <compound_statement> |<condition_statement> |<loop_statement> |<jump_statement> |<print_statement> |<scan_statement> |<assignment_expression>';' |<function_call>';' |';'
 //todo:
    std::optional<CompilationError> Analyser::statement(const std::string & whilename, const std::string & funname,  SymbleTable *symbleTable, int32_t returntype) {
        //compound_statement 作用域_语句
        auto ed = nextToken();
        if(ed.has_value() && ed.value().GetType() == TokenType::BIG_LEFT_BRACKET){
            unreadToken();
            auto err = compound_statement(whilename, funname, symbleTable, returntype);
            if (err.has_value())
                return err;


        }
        //赋值_表达式  assignment_expression 函数_call
        else if(ed.has_value() && ed.value().GetType() == TokenType::IDENTIFIER) {
            std::string name = ed.value().GetValueString();
            int32_t kind = symbleTable->getKindByName(name);
            if(kind == 1){//赋值_表达式
                unreadToken();
                auto err = assignment_expression(funname, symbleTable);
                if(err.has_value()) return err;
            }else if(functionIndex(name) >= 0){//函数_call
                unreadToken();
                auto err = function_call(funname, symbleTable);
                if(err.has_value())
                    return err;
            } else if(kind == 0)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrSetConst);
            else
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);

            ed = nextToken();
            if(!ed.has_value() || ed.value().GetType() != TokenType::SEMICOLON)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        }
        else if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "scan") {
            unreadToken();
            auto err = scan_statement(funname, symbleTable);
            if(err.has_value()) return err;
        }
        else if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "print") {
            unreadToken();
            auto err = print_statement(funname, symbleTable);
            if(err.has_value()) return err;
        }

        else if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "return") {
            //std::printf("now here \n");
            if (returntype == 0) {
                addInstructionByFunName( Operation::ret, 0, 0,funname);
            }
            if (returntype == 1) {
                auto err = expression(funname, true, symbleTable);
                if (err.has_value())
                    return err;
                addInstructionByFunName( Operation::iret, 0, 0, funname);
            }
            if (returntype == 2) {
                auto err = expression(funname, false, symbleTable);
                if (err.has_value())
                    return err;
                addInstructionByFunName( Operation::iret, 0, 0, funname);
            }
            ed = nextToken();
            if(!ed.has_value() || ed.value().GetType() != TokenType::SEMICOLON)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        }
        else if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "if") {
            unreadToken();
            auto err = condition_statement(whilename, funname,  symbleTable, returntype);
            if (err.has_value())
                return err;
        }
        else if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "while") {
            unreadToken();
            auto err = while_statement(funname,  symbleTable, returntype);
            if (err.has_value())
                return err;
        }else if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "break"){
            ed = nextToken();
            if(!ed.has_value() || ed.value().GetType() != TokenType::SEMICOLON)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);

            if(whilename == "")
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrBreakNoneLoop);

            int32_t count = getCountByFunName(funname);
            addInstructionByFunName(Operation::jmp, 0, 0, funname);
            addWhileBreak(whilename,count);
        }else if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "continue"){
            ed = nextToken();
            if(!ed.has_value() || ed.value().GetType() != TokenType::SEMICOLON)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);

            if(whilename == "")
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrContinueNoneLoop);

            int32_t count = getCountByFunName(funname);
            addInstructionByFunName(Operation::jmp, 0, 0, funname);
            addWhileContinue(whilename,count);
        }else if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "do") {
            unreadToken();
            auto err = do_while_statement(funname,  symbleTable, returntype);
            if (err.has_value())
                return err;
        }else if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "for") {
            unreadToken();
            auto err = for_statement(funname,  symbleTable, returntype);
            if (err.has_value())
                return err;
        }




        //todo:



        return {};
    }


    std::optional<CompilationError> Analyser::compound_statement(const std::string & whilename, const std::string & funname,  SymbleTable *symbleTable, int32_t funtype) {
        auto ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::BIG_LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);


        std::string uuid = newAUuid();
        SymbleTable *newsymbleTable = new SymbleTable(symbleTable,uuid);

        ed = nextToken();
        while(ed.has_value() && ed.value().GetType() == RESERVED_WORD &&
              (ed.value().GetValueString() == "const" ||
               ed.value().GetValueString() == "int" ||
               ed.value().GetValueString() == "char")){
            unreadToken();
            variable_declaration(funname, newsymbleTable);
            ed = nextToken();
        }

        while(ed.has_value() && ed.value().GetType() != BIG_RIGHT_BRACKET ) {/*
            if (ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD &&
                ed.value().GetValueString() == "return") {
                //std::printf("now here \n");
                int32_t returntype = functionType(funname);
                if (returntype == 0) {
                    addInstructionByFunName(Operation::ret, 0, 0,funname);
                }
                if (returntype == 1) {
                    auto err = expression(funname, true, newsymbleTable);
                    if (err.has_value())
                        return err;
                    addInstructionByFunName(Operation::iret, 0, 0,funname);
                }
                if (returntype == 2) {
                    auto err = expression(funname, false, newsymbleTable);
                    if (err.has_value())
                        return err;
                    addInstructionByFunName(Operation::iret, 0, 0,funname);
                }
                ed = nextToken();
                if (!ed.has_value() || ed.value().GetType() != TokenType::SEMICOLON)
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
            }
                else {*/
                    unreadToken();
                    auto err = statement(whilename, funname, newsymbleTable, funtype);
                    if (err.has_value())
                        return err;
                    ed = nextToken();
                //}

        }
        if(!ed.has_value())
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);

        return {};
    }
/*********************************print scan *************************************************/
//<print_语句> ::=  'print' '(' [<printable_列表>] ')' ';'
//<print_statement> ::=  'print' '(' [<printable_list>] ')' ';'
//<printable_列表>  ::=  <printable> {',' <printable>}
//<printable_list>  ::=  <printable> {',' <printable>}
    std::optional<CompilationError> Analyser::print_statement(const std::string &funname, SymbleTable *symbleTable) {
        auto ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::RESERVED_WORD || ed.value().GetValueString() != "print") {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
        }
        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::LEFT_BRACKET) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);
        }

        auto err = printable(funname, symbleTable);
        if(err.has_value())
            return err;

        ed = nextToken();
        while(ed.has_value() && ed.value().GetType() == TokenType::COMMA_SIGN){
            auto err = printable(funname, symbleTable);
            if(err.has_value())
                return err;
            ed = nextToken();
        }


        if(!ed.has_value() || ed.value().GetType() != TokenType::RIGHT_BRACKET) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
        }

        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::SEMICOLON) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        }
        return {};
    }
    //<printable> ::=  <表达式> | <string_串>| <char-literal>
//<printable> ::=  <expression> | <string_literal>| <char-literal>
    std::optional<CompilationError> Analyser::printable(const std::string &funname, SymbleTable *symbleTable) {
        auto ed = nextToken();

        if(ed.has_value() && ed.value().GetType() == TokenType::CHAR_SIGN){
            char ch = ed.value().GetValueString()[0];
            addInstructionByFunName( Operation::ipush,(int)ch, 0, funname);
            addInstructionByFunName( Operation::cprint,0 , 0, funname);


        }else if(ed.has_value() && ed.value().GetType() == TokenType::STRING_SIGN){
                addString(ed.value().GetValueString());
                int32_t index = stringIndex(ed.value().GetValueString());
                if(index < 0)
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
                addInstructionByFunName( Operation::loadc, index, 0, funname);
                addInstructionByFunName( Operation::sprint, 0, 0, funname);

        }else{
            bool isint;
            unreadToken();
            auto err = print_expression(funname,&isint,symbleTable);
            if(err.has_value())
                return err;
            if(isint){
                addInstructionByFunName( Operation::iprint, 0, 0, funname);
            }else{
                addInstructionByFunName( Operation::cprint, 0, 0, funname);
            }
        }
        return {};
    }
    std::optional<CompilationError> Analyser::print_expression(const std::string &funname, bool *isINT, SymbleTable *symbleTable) {
        auto err = additive_expression(funname, isINT, symbleTable);
        if (err.has_value())
            return err;
        return {};
    }
    //<scan_语句> ::=  'scan' '(' <标识符> ')' ';'
//<scan_statement> ::=  'scan' '(' <identifier> ')' ';'
    std::optional<CompilationError> Analyser::scan_statement(const std::string &funname, SymbleTable *symbleTable) {
        auto ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::RESERVED_WORD || ed.value().GetValueString() != "scan") {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
        }
        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::LEFT_BRACKET) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);
        }

        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
        std::string name = ed.value().GetValueString();
        int32_t kind = symbleTable->getKindByName(name);
        if(kind == 0)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrSetConst);
        else if(kind != 1)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);

        std::pair<int32_t ,int32_t > pii = symbleTable->index_in_all(name);
        addInstructionByFunName( Operation::loada, pii.second, pii.first, funname);

        int32_t type = symbleTable->getTypeByName(name);
        if(type == 1) {//int
            addInstructionByFunName( Operation::iscan, 0, 0, funname);
            addInstructionByFunName( Operation::istore, 0, 0, funname);
        }else if (type == 2) {//char
            addInstructionByFunName( Operation::cscan, 0, 0, funname);
            addInstructionByFunName( Operation::istore, 0, 0, funname);
        }else
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);


        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::RIGHT_BRACKET) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
        }

        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::SEMICOLON) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        }

        symbleTable->setValueByName(name);
        return {};
    }
/********************************************if ********************************************************/
//<condition_statement> ::=
//    'if' '(' <condition> ')' <statement> ['else' <statement>]

    std::optional<CompilationError> Analyser::condition_statement(const std::string & whilename, const std::string & funname, SymbleTable *symbleTable, int32_t returntype) {
        auto ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::RESERVED_WORD || ed.value().GetValueString() != "if") {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
        }
        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::LEFT_BRACKET) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);
        }

        int32_t count;
        auto err = condition(funname, symbleTable,&count);
        if(err.has_value())
            return err;

        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::RIGHT_BRACKET) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
        }

        err = statement(whilename, funname, symbleTable, returntype);
        if(err.has_value())
            return err;

        ed = nextToken();
        if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "else") {
            int32_t newcount = getCountByFunName(funname);
            addInstructionByFunName(Operation::jmp, 0, 0, funname);
            changeXByCountAndFunName(count, newcount, funname);
            err = statement(whilename, funname, symbleTable, returntype);
            if(err.has_value())
                return err;
            changeXByCountAndFunName(newcount, getCountByFunName(funname)-1, funname);
        } else{
            unreadToken();
            changeXByCountAndFunName(count, getCountByFunName(funname)-1, funname);
        }

        return {};
    }
//<条件> ::=   <表达式>[<关系_操作符><表达式>]
//<condition> ::=   <expression>[<relational_operator><expression>]
    std::optional<CompilationError> Analyser::condition(const std::string &funname, SymbleTable *symbleTable,int32_t * index) {
        auto err = expression(funname, true, symbleTable);
        if(err.has_value())
            return err;
        auto ed = nextToken();
        if(ed.has_value() && ed.value().GetType() == TokenType::DOUBLE_EQUAL_SIGN) {
            err = expression(funname, true, symbleTable);
            if(err.has_value())
                return err;

            addInstructionByFunName( Operation::isub, 0, 0, funname);
            *index = getCountByFunName(funname);
            addInstructionByFunName(Operation::jne, 0, 0, funname);

        }else if(ed.has_value() && ed.value().GetType() == TokenType::GREATER_SIGN) {
            err = expression(funname, true, symbleTable);
            if(err.has_value())
                return err;

            addInstructionByFunName( Operation::isub, 0, 0, funname);
            *index = getCountByFunName(funname);
            addInstructionByFunName(Operation::jle, 0, 0, funname);

        }else if(ed.has_value() && ed.value().GetType() == TokenType::GREATER_EQUAL_SIGN) {
            err = expression(funname, true, symbleTable);
            if(err.has_value())
                return err;

            addInstructionByFunName( Operation::isub, 0, 0, funname);
            *index = getCountByFunName(funname);
            addInstructionByFunName(Operation::jl, 0, 0, funname);

        }else if(ed.has_value() && ed.value().GetType() == TokenType::LESS_SIGN) {
            err = expression(funname, true, symbleTable);
            if(err.has_value())
                return err;

            addInstructionByFunName( Operation::isub, 0, 0, funname);
            *index = getCountByFunName(funname);
            addInstructionByFunName(Operation::jge, 0, 0, funname);

        }else if(ed.has_value() && ed.value().GetType() == TokenType::LESS_EQUAL_SIGN) {
            err = expression(funname, true, symbleTable);
            if(err.has_value())
                return err;

            addInstructionByFunName( Operation::isub, 0, 0, funname);
            *index = getCountByFunName(funname);
            addInstructionByFunName(Operation::jg, 0, 0, funname);

        }else if(ed.has_value() && ed.value().GetType() == TokenType::UNEQUAL_SIGN) {
            err = expression(funname, true, symbleTable);
            if(err.has_value())
                return err;

            addInstructionByFunName( Operation::isub, 0, 0, funname);
            *index = getCountByFunName(funname);
            addInstructionByFunName(Operation::je, 0, 0, funname);

        }else{
            unreadToken();
            *index = getCountByFunName(funname);
            addInstructionByFunName(Operation::je, 0, 0, funname);
        }
    return {};
    }
  //<while_statement> ::=
    //    'while' '(' <condition> ')' <statement>
  std::optional<CompilationError> Analyser::while_statement(const std::string &funname, SymbleTable *symbleTable, int32_t returntype) {
      auto ed = nextToken();
      if(!ed.has_value() || ed.value().GetType() != TokenType::RESERVED_WORD || ed.value().GetValueString() != "while") {
          return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
      }
      ed = nextToken();
      if(!ed.has_value() || ed.value().GetType() != TokenType::LEFT_BRACKET) {
          return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);
      }

      std::string whilename = addWhile();
      int oldcount = getCountByFunName(funname)-1;
      int count;
      auto err = condition(funname,symbleTable,&count);
      if(err.has_value())
          return err;

      ed = nextToken();
      if(!ed.has_value() || ed.value().GetType() != TokenType::RIGHT_BRACKET) {
          return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
      }


      err = statement(whilename, funname, symbleTable, returntype);
      if(err.has_value())
          return err;

      addInstructionByFunName(Operation::jmp, oldcount, 0, funname);

      changeXByCountAndFunName(count, getCountByFunName(funname)-1, funname);

      std::vector<int32_t> vec = getWhileBreaksByName(whilename);
      for(auto v : vec){
          changeXByCountAndFunName(v, getCountByFunName(funname)-1, funname);
      }

      vec = getWhileContinuesByName(whilename);
      for(auto v : vec){
          changeXByCountAndFunName(v, oldcount, funname);
      }
        return {};
    }


    //'do' <statement> 'while' '(' <condition> ')' ';'
    std::optional<CompilationError> Analyser::do_while_statement(const std::string &funname, SymbleTable *symbleTable, int32_t returntype) {

        auto ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::RESERVED_WORD || ed.value().GetValueString() != "do") {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
        }

        std::string whilename = addWhile();
        int oldcount = getCountByFunName(funname)-1;

        auto err = statement(whilename, funname, symbleTable, returntype);
        if(err.has_value())
            return err;


        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::RESERVED_WORD || ed.value().GetValueString() != "while") {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
        }
        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::LEFT_BRACKET) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);
        }

        int count;
        err = condition(funname,symbleTable,&count);
        if(err.has_value())
            return err;

        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::RIGHT_BRACKET) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
        }

        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::SEMICOLON) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        }

        addInstructionByFunName(Operation::jmp, oldcount, 0, funname);

        changeXByCountAndFunName(count, getCountByFunName(funname)-1, funname);

        std::vector<int32_t> vec = getWhileBreaksByName(whilename);
        for(auto v : vec){
            changeXByCountAndFunName(v, getCountByFunName(funname)-1, funname);
        }

        vec = getWhileContinuesByName(whilename);
        for(auto v : vec){
            changeXByCountAndFunName(v, oldcount, funname);
        }
        return {};
    }

    //todo:
//'for' '('<for_init_statement> [<condition>]';' [<for_update_expression>]')' <statement>
    std::optional<CompilationError> Analyser::for_statement(const std::string &funname, SymbleTable *symbleTable, int32_t returntype) {
        auto ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::RESERVED_WORD || ed.value().GetValueString() != "for") {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
        }

        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::LEFT_BRACKET) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);
        }

        std::string whilename = addWhile();

        auto err = for_init_statement(funname,symbleTable);
        if(err.has_value())
            return err;

        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::SEMICOLON) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        }

        int tocount2 = getCountByFunName(funname);
        int fromcount4;

        ed = nextToken();
        unreadToken();
        bool hasCondition = false;
        if(!ed.has_value() || ed.value().GetType() != TokenType::SEMICOLON) {
            err = condition(funname,symbleTable,&fromcount4);
            hasCondition = true;
            if(err.has_value())
                return err;
        }
        int fromcount3 = getCountByFunName(funname);
        addInstructionByFunName(Operation::jmp, 0, 0, funname);

        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::SEMICOLON) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        }

        int tocount1 = getCountByFunName(funname);

        ed = nextToken();
        unreadToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::RIGHT_BRACKET) {
            err = for_update_expression(funname,symbleTable);
            if(err.has_value())
                return err;
        }

        int fromcount2 = getCountByFunName(funname);
        addInstructionByFunName(Operation::jmp, 0, 0, funname);

        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::RIGHT_BRACKET) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
        }

        int tocount3 = getCountByFunName(funname);
        err = statement(whilename,funname,symbleTable,returntype);
        if(err.has_value())
            return err;
        int fromcount1 = getCountByFunName(funname);
        addInstructionByFunName(Operation::jmp, 0, 0, funname);


        int tocount4 = getCountByFunName(funname);
        changeXByCountAndFunName(fromcount1, tocount1 -1, funname);
        changeXByCountAndFunName(fromcount2, tocount2 -1, funname);
        changeXByCountAndFunName(fromcount3, tocount3 -1, funname);
        if(hasCondition) changeXByCountAndFunName(fromcount4, tocount4 -1, funname);

        std::vector<int32_t> vec = getWhileBreaksByName(whilename);
        for(auto v : vec){
            changeXByCountAndFunName(v, tocount4 -1, funname);
        }
        vec = getWhileContinuesByName(whilename);
        for(auto v : vec){
            changeXByCountAndFunName(v, tocount1 -1, funname);
        }
        return {};
    }

    std::optional<CompilationError> Analyser::for_init_statement(const std::string &funname, SymbleTable *symbleTable) {
        auto err = assignment_expression(funname, symbleTable);
        if(err.has_value())
            return err;
        auto ed = nextToken();
        while(ed.has_value() && ed.value().GetType() == TokenType::COMMA_SIGN){
            auto err = assignment_expression(funname, symbleTable);
            if(err.has_value())
                return err;
            ed = nextToken();
        }

        unreadToken();
        return {};
    }
    std::optional<CompilationError> Analyser::for_update_expression(const std::string &funname, SymbleTable *symbleTable) {

        auto next = nextToken();
        std::string str = next.value().GetValueString();
        if(next.has_value() && next.value().GetType() == TokenType::IDENTIFIER){
            next = nextToken();
            if(next.has_value() && next.value().GetType() == TokenType::LEFT_BRACKET){//函数
                if(functionIndex(str) >= 0){
                    //<函数_call>
                    unreadToken();
                    unreadToken();
                    int32_t  type = functionType(str);
                    if(type == 0)
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrVoidFun);
                    auto err = function_call(funname, symbleTable);
                    if(err.has_value())
                        return err;
                }else
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
            }else{//字符
                unreadToken();
                unreadToken();
                int32_t kind = symbleTable->getKindByName(str);

                if(kind == 1){
                    if(symbleTable->getValueByName(str) == 0)
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotInitialized);

                    std::pair<int32_t ,int32_t > pii = symbleTable->index_in_all(str);
                    if(pii.first == -1)
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
                    int32_t type = symbleTable->getTypeByName(str);
                    if(type == 1) {
                        addInstructionByFunName( Operation::loada, pii.second,pii.first, funname);//加载地址
                        addInstructionByFunName( Operation::iload, 0,0, funname);//得到数
                    }
                    else if(type == 2 ){//char
                        addInstructionByFunName( Operation::loada, pii.second,pii.first, funname);//加载地址
                        addInstructionByFunName( Operation::iload, 0,0, funname);//得到数
                    }
                    else
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
                }else
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);


                auto err = assignment_expression(funname, symbleTable);
                if(err.has_value())
                    return err;


            }
        }else return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);


        next = nextToken();
        while(next.has_value() && next.value().GetType() == TokenType::COMMA_SIGN){
            auto err = assignment_expression(funname, symbleTable);
            if(err.has_value())
                return err;



            next = nextToken();
            std::string str = next.value().GetValueString();
            if(next.has_value() && next.value().GetType() == TokenType::IDENTIFIER){
                next = nextToken();
                if(next.has_value() && next.value().GetType() == TokenType::LEFT_BRACKET){//函数
                    if(functionIndex(str) >= 0){
                        //<函数_call>
                        unreadToken();
                        unreadToken();
                        int32_t  type = functionType(str);
                        if(type == 0)
                            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrVoidFun);
                        auto err = function_call(funname, symbleTable);
                        if(err.has_value())
                            return err;
                    }else
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
                }else{//字符
                    unreadToken();
                    int32_t kind = symbleTable->getKindByName(str);

                    if(kind == 0 || kind == 1){
                        if(symbleTable->getValueByName(str) == 0)
                            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotInitialized);

                        std::pair<int32_t ,int32_t > pii = symbleTable->index_in_all(str);
                        if(pii.first == -1)
                            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
                        int32_t type = symbleTable->getTypeByName(str);
                        if(type == 1) {
                            addInstructionByFunName( Operation::loada, pii.second,pii.first, funname);//加载地址
                            addInstructionByFunName( Operation::iload, 0,0, funname);//得到数
                        }
                        else if(type == 2 ){//char
                            addInstructionByFunName( Operation::loada, pii.second,pii.first, funname);//加载地址
                            addInstructionByFunName( Operation::iload, 0,0, funname);//得到数
                        }
                        else
                            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
                    }else
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
                }
            }



            next = nextToken();
        }

        unreadToken();
        return {};
    }


/*********************************assignment_expression******************************************/
    //<赋值_表达式> ::=  <标识符><赋值_操作符><表达式>
    //<assignment_expression> ::=  <identifier><assignment_operator><expression>
    std::optional<CompilationError> Analyser::assignment_expression(const std::string &funname, SymbleTable *symbleTable) {
        auto ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
        std::string name = ed.value().GetValueString();


        if(symbleTable->getKindByName(name) == -1)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
        int type = symbleTable->getTypeByName(name);
        symbleTable->setValueByName(name);

        // =
        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::EQUAL_SIGN)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoEqualSign);

        std::pair<int32_t ,int32_t > pii = symbleTable->index_in_all(name);
        //对栈操作
        addInstructionByFunName( Operation::loada, pii.second, pii.first, funname);

        auto err = expression(funname, type == 1,symbleTable);
        if(err.has_value())
            return err;

        if(type == 1)
            addInstructionByFunName( Operation::istore, 0, 0, funname);
        else
            addInstructionByFunName( Operation::istore, 0, 0, funname);

        return {};
    }


/***********************************function_call*********************************************/

//<函数_call> ::=  <标识符> '(' [<表达式_列表>] ')'
//<function_call> ::=  <identifier> '(' [<expression_list>] ')'
//<表达式_列表> ::=  <表达式>{','<表达式>}
//<expression_list> ::=  <expression>{','<expression>}
    std::optional<CompilationError> Analyser::function_call(const std::string &funname, SymbleTable *symbleTable) {

        auto ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
        std::string name = ed.value().GetValueString();
        int index = functionIndex(name);
        if(index < 0)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);

        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() !=TokenType::LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);

        std::vector< std::pair<std::string, std::pair<std::int32_t , int32_t > > >params = _functions[index].getParams();

        for(unsigned long long int i = 0; i < params.size();i++){
            bool isINT = params[i].second.second ==1;
            auto err = expression(funname, isINT,symbleTable);
            if(err.has_value())
                return err;
            if(i < params.size()-1)
            {
                ed = nextToken();
                if(!ed.has_value() || ed.value().GetType() != TokenType::COMMA_SIGN)
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrLessParams);
            }
        }
        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() !=TokenType::RIGHT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);

        addInstructionByFunName( Operation::call, index, 0, funname);
        return {};
    }


/************************************expression**********************************/


//<表达式> ::=  <加法_表达式>
//<expression> ::=  <additive_expression>
    std::optional<CompilationError> Analyser::expression(const std::string &funname, bool isINT, SymbleTable *symbleTable) {
        bool isintbegin = isINT;
        bool isint = isINT;
        auto err = additive_expression(funname, &isint,symbleTable);
        if (err.has_value())
            return err;
        if(!isintbegin && isint)
            addInstructionByFunName( Operation::i2c , 0, 0, funname);
        return {};
    }
//<加法_表达式> ::=   <乘法_表达式>{<加法_操作符><乘法_表达式>}
//<additive_expression> ::=   <multiplicative_expression>{<additive_operator><multiplicative_expression>}
    std::optional<CompilationError> Analyser::additive_expression(const std::string &funname, bool *isINT, SymbleTable *symbleTable) {
        auto err = multiplicative_expression(funname, isINT,symbleTable);
        if (err.has_value())
            return err;

        // {<加法型运算符><项>}
        while (true) {
            // 预读
            auto next = nextToken();
            if (!next.has_value())
                return {};
            auto type = next.value().GetType();
            if (type != TokenType::PLUS_SIGN && type != TokenType::MINUS_SIGN) {
                unreadToken();
                return {};
            }

            // <项>
            err = multiplicative_expression(funname, isINT,symbleTable);
            if (err.has_value())
                return err;

            if (type == TokenType::PLUS_SIGN)
                addInstructionByFunName( Operation::iadd , 0, 0, funname);
            else if (type == TokenType::MINUS_SIGN)
                addInstructionByFunName( Operation::isub, 0, 0, funname);
        }
        return {};
    }
//<乘法_表达式> ::=   <cast_表达式>{<乘法_操作符><cast_表达式>}
//<multiplicative_expression> ::=   <cast_expression>{<multiplicative_operator><cast_expression>}
    std::optional<CompilationError> Analyser::multiplicative_expression(const std::string &funname, bool *isINT, SymbleTable *symbleTable) {
        auto err = cast_expression(funname, isINT,symbleTable);
         if (err.has_value())
            return err;

        // { <乘法型运算符><因子> }
        while (true) {
            // 预读
            auto next = nextToken();
            if (!next.has_value())
                return {};
            auto type = next.value().GetType();
            if (type != TokenType::MULTIPLICATION_SIGN && type != TokenType::DIVISION_SIGN) {
                unreadToken();
                return {};
            }

            // <因子>
            err = cast_expression(funname, isINT,symbleTable);
            if (err.has_value())
                return err;

            if (type == TokenType::MULTIPLICATION_SIGN)
                addInstructionByFunName( Operation::imul, 0, 0, funname);
            else if (type == TokenType::DIVISION_SIGN)
                addInstructionByFunName( Operation::idiv, 0, 0, funname);
        }
        return {};
    }

//<cast_表达式> ::= {'('<类型_说明符号>')'}<单目_表达式>
//<cast_expression> ::= {'('<type_specifier>')'}<primary_expression>
    std::optional<CompilationError> Analyser::cast_expression(const std::string &funname, bool *isINT, SymbleTable *symbleTable) {
        auto next = nextToken();
        //{'('<type_specifier>')'}
        if(next.has_value() && next.value().GetType() == LEFT_BRACKET){
            bool nowINT = *isINT;
            next = nextToken();
            if(next.has_value() && next.value().GetType() == RESERVED_WORD && next.value().GetValueString()=="char"){
                nowINT = false;
            }else if(next.has_value() && next.value().GetType() == RESERVED_WORD && next.value().GetValueString()=="int"){
                nowINT = true;
            }else return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoType_specifier);
            next = nextToken();
            if(!next.has_value() || next.value().GetType() != RIGHT_BRACKET)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
            next = nextToken();
            while(next.has_value() && next.value().GetType() == LEFT_BRACKET) {
                next = nextToken();
                if (next.has_value() && next.value().GetType() == RESERVED_WORD &&
                    next.value().GetValueString() == "char") {
                } else if (next.has_value() && next.value().GetType() == RESERVED_WORD &&
                           next.value().GetValueString() == "int") {
                } else return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoType_specifier);
                next = nextToken();
                if (!next.has_value() || next.value().GetType() != RIGHT_BRACKET)
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
                next = nextToken();
            }

            unreadToken();
            auto err = primary_expression(funname, isINT,symbleTable);
            if (err.has_value())
                return err;

            *isINT = nowINT;
        }else{
            unreadToken();
            auto err = primary_expression(funname, isINT,symbleTable);
            if (err.has_value())
                return err;
        }

        //*isINT = nowINT;
        return {};
    }

//<primary_表达式> ::=   [<单目_操作符>]     '('<表达式>')'  |<标识符> |<整数_串> |<char_串> |<浮点数_串> |<函数_call>
//<primary_expression> ::=   [<unary_operator>]     '('<expression>')'  |<identifier> |<integer_literal> |<char_literal> |<floating_literal> |<function_call>
    std::optional<CompilationError> Analyser::primary_expression(const std::string &funname, bool *isINT, SymbleTable *symbleTable) {
// [<单目_操作符>]
        auto next = nextToken();
        auto prefix = 1;
        if (!next.has_value())
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
        if (next.value().GetType() == TokenType::PLUS_SIGN)
            prefix = 1;
        else if (next.value().GetType() == TokenType::MINUS_SIGN) {
            prefix = -1;
        } else
            unreadToken();

        // 预读
        next = nextToken();
        if (!next.has_value())
            return {};
            //return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
        switch (next.value().GetType()) {
            //<标识符>
            case TokenType::IDENTIFIER: {
                std::string str = next.value().GetValueString();

                next = nextToken();
                if(next.has_value() && next.value().GetType() == TokenType::LEFT_BRACKET){//函数
                    if(functionIndex(str) >= 0){
                        //<函数_call>
                        unreadToken();
                        unreadToken();
                        int32_t  type = functionType(str);
                        if(type == 1) *isINT = true;
                        if(type == 0)
                            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrVoidFun);
                        auto err = function_call(funname, symbleTable);
                        if(err.has_value())
                            return err;
                    }else
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
                }else{//字符
                    unreadToken();
                    int32_t kind = symbleTable->getKindByName(str);

                    if(kind == 0 || kind == 1){
                        if(symbleTable->getValueByName(str) == 0)
                            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotInitialized);

                        std::pair<int32_t ,int32_t > pii = symbleTable->index_in_all(str);
                        if(pii.first == -1)
                            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
                        int32_t type = symbleTable->getTypeByName(str);
                        if(type == 1) {
                            *isINT = true;
                            addInstructionByFunName( Operation::loada, pii.second,pii.first, funname);//加载地址
                            addInstructionByFunName( Operation::iload, 0,0, funname);//得到数
                        }
                        else if(type == 2 ){//char
                            addInstructionByFunName( Operation::loada, pii.second,pii.first, funname);//加载地址
                            addInstructionByFunName( Operation::iload, 0,0, funname);//得到数
                        }
                        else
                            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
                        break;
                    }else
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
                }

                break;
            }

                //<无符号整数>
            case TokenType::UNSIGNED_INTEGER: {
                *isINT = true;
                std::string numstr = next.value().GetValueString();
                std::stringstream myss;
                myss << numstr;
                int32_t num = 0;
                myss >> num;
                addInstructionByFunName( Operation::ipush, num, 0, funname);
                break;
            }
                //'('<表达式>')'
            case TokenType::LEFT_BRACKET: {
                if (!next.has_value())
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
                auto err = expression(funname, isINT,symbleTable);
                if (err.has_value())
                    return err;
                next = nextToken();
                if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
                break;
            }
            //<char_串>
            case TokenType::CHAR_SIGN:{
                std::string string = next.value().GetValueString();
                char c;
                sscanf(string.c_str(),"%c",&c);
                auto int32 = (int32_t)c;
                addInstructionByFunName(Operation::bipush, int32, 0, funname);
                break;
            }




                // 这里和 <语句序列> 类似，需要根据预读结果调用不同的子程序
                // 但是要注意 default 返回的是一个编译错误
            default:
                return{};
                //return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
        }

        // 取负
        if(prefix == -1) {
            addInstructionByFunName( Operation::ineg , 0, 0, funname);
            *isINT = true;
        }
        return {};
    }



    std::optional<Token> Analyser::nextToken() {
        if (_offset == _tokens.size())
            return {};
        // 考虑到 _tokens[0..._offset-1] 已经被分析过了
        // 所以我们选择 _tokens[0..._offset-1] 的 EndPos 作为当前位置
        _current_pos = _tokens[_offset].GetEndPos();
        return _tokens[_offset++];
    }

    void Analyser::unreadToken() {
        if (_offset == 0)
            DieAndPrint("analyser unreads token from the begining.");
        _current_pos = _tokens[_offset - 1].GetEndPos();
        _offset--;
    }

    void Analyser::addString(const std::string& string) {
        if(stringIndex(string) < 0){
            _stringTable.push_back(string);
        }
    }
    int32_t Analyser::stringIndex(const std::string& string){
        for(unsigned long long int i = 0; i < _stringTable.size(); i++){
            if(string == _stringTable[i])
                return i;
        }
        return -1;
    }

    bool Analyser::addParamByFunName(const std::string& param,const int32_t kind,const int32_t type, const std::string& name){
        for(auto & _function : _functions){
            if(name == _function.getName()){
                return _function.addParam(param,kind, type);
            }
        }
        return false;
    }

    std::string Analyser::addWhile(){
        std::string string = newAUuid();
        _while_break_count.emplace_back(string, std::vector<int32_t >());
        _while_countinue_count.emplace_back(string, std::vector<int32_t >());
        return string;
    }

    void Analyser::addWhileBreak(const std::string& name,  const int32_t count){
        int i = 0;
        for(auto v : _while_break_count){
            if(name == v.first){
                _while_break_count[i].second.push_back(count);
                return;
            }
            i++;
        }
    }

    std::vector<int32_t> Analyser::getWhileBreaksByName(const std::string& name){
        for(auto v : _while_break_count){
            if(name == v.first){
                return v.second;
            }
        }
        return std::vector<int32_t>();
    }
    void Analyser::addWhileContinue(const std::string& name, const int32_t count){
        int i = 0;
        for(auto v : _while_countinue_count){
            if(name == v.first){
                _while_countinue_count[i].second.push_back(count);
                return;
            }
            i++;
        }
    }

    std::vector<int32_t> Analyser::getWhileContinuesByName(const std::string& name){
        for(auto v : _while_countinue_count){
            if(name == v.first){
                return v.second;
            }
        }
        return std::vector<int32_t>();
    }

    bool Analyser::addInstructionByFunName(Operation operation, int32_t x, int32_t y, const std::string& name){
        if(name =="")
            _instructions.emplace_back(__start_count++,operation, x, y);
        for(auto & _function : _functions){
            if(name == _function.getName()){
                _function.addInstruction(operation,x,y);
                return true;
            }
        }
        return false;
    }

    int32_t Analyser::getCountByFunName(const std::string &name){
        if(name =="")
            return __start_count;
        for(auto & _function : _functions){
            if(name == _function.getName()){
                return _function.getCount();
            }
        }
        return -1;
    }

    bool Analyser::changeXByCountAndFunName(int32_t count,int32_t x,const std::string& name){
        //if(count == 0) return false;
        if(name =="")
            _instructions[count].setX(x);
        for(auto & _function : _functions){
            if(name == _function.getName()){
                _function.setInstructionX(count, x);
                return true;
            }
        }
        return false;
    }

    int32_t Analyser::functionIndex(const std::string& string){
        for(unsigned long long int i = 0; i < _functions.size(); i++){
            if(string == _functions[i].getName())
                return i;
        }
        return -1;
    }
    int32_t Analyser::functionType(const std::string& string){
        for(unsigned long long int i = 0; i < _functions.size(); i++){
            if(string == _functions[i].getName())
                return _functions[i].getType();
        }
        return -1;
    }
    bool Analyser::addFun( const std::string& name, const int32_t type, const int32_t name_index, const int32_t level){
        if(functionIndex(name) >= 0) return false;
        _functions.emplace_back(name, type,name_index,level);
        return true;
    }

    std::string Analyser::newAUuid(){
        static char buf[64] = {0};
        GUID guid;
        if (S_OK == ::CoCreateGuid(&guid))
        {
            _snprintf(buf, sizeof(buf)
                    , "{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"
                    , (unsigned int)guid.Data1
                    , guid.Data2
                    , guid.Data3
                    , guid.Data4[0], guid.Data4[1]
                    , guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
                    , guid.Data4[6], guid.Data4[7]
            );
        }
        return (const char*)buf;

    }

    const std::vector<std::string> &Analyser::getStringTable() const {
        return _stringTable;
    }

    const std::vector<function> &Analyser::getFunctions() const {
        return _functions;
    }
}