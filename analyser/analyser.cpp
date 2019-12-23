#include "analyser.h"

#include <climits>
#include <sstream>

namespace miniplc0 {

    std::pair<std::vector<Instruction>, std::optional<CompilationError>> Analyser::Analyse() {
        auto err = C0_program();
        if (err.has_value())
            return std::make_pair(std::vector<Instruction>(), err);
        else
            return std::make_pair(_instructions, std::optional<CompilationError>());
    }


    //<C0-program> ::=
    //        {<变量_声明>}{<函数_定义>}
    //<C0_program> ::=
    //    {<variable_declaration>}{<function_definition>}
    std::optional<CompilationError> Analyser::C0_program() {
        auto ed = nextToken();
        while(ed.has_value()){
            if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD){
                if(ed.value().GetValueString().compare("const")){
                    unreadToken();
                    auto err = variable_declaration(&_symbleTable);
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
                    if(ed.has_value() && (ed.value().GetType() == TokenType::EQUAL_SIGN|| ed.value().GetType() == TokenType::COMMA_SIGN)){
                        unreadToken();
                        unreadToken();
                        auto err = init_declarator_list(false, false,&_symbleTable);
                        if (err.has_value())
                            return err;
                    }else if(ed.has_value() && ed.value().GetType() == TokenType::LEFT_BRACKET){

                        unreadToken();
                        unreadToken();
                        function_definition();
                    }


                }else if (ed.value().GetValueString().compare("int")){
                    ed = nextToken();
                    if(!ed.has_value() || !(ed.value().GetType() == TokenType::IDENTIFIER))
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);

                    ed = nextToken();
                    if(ed.has_value() && (ed.value().GetType() == TokenType::EQUAL_SIGN|| ed.value().GetType() == TokenType::COMMA_SIGN)){
                        unreadToken();
                        unreadToken();
                        auto err = init_declarator_list(false, true, &_symbleTable);
                        if (err.has_value())
                            return err;
                    }else if(ed.has_value() && ed.value().GetType() == TokenType::LEFT_BRACKET){

                        unreadToken();
                        unreadToken();
                        function_definition();
                    }
                }else
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoType_specifier);
            }
            auto err = variable_declaration(&_symbleTable);
            if (err.has_value())
                return err;
            err = function_definition();
            if (err.has_value())
                return err;
        }
        return {};
    }


    //<变量_声明> ::=
    //    [<const_修饰符>]<类型_说明符号><声明_语句_列表>';'
    //<variable_declaration> ::=
    //    [<const_qualifier>]<type_specifier><init_declarator_list>';'
    std::optional<CompilationError> Analyser::variable_declaration(SymbleTable *symbleTable) {
        //[<const_qualifier>]
        bool isCONST = true;
        auto ed = nextToken();
        if (!ed.has_value() || ed.value().GetType() != TokenType::RESERVED_WORD || !ed.value().GetValueString().compare("const")){
            isCONST = false;
            unreadToken();
        }
        //<type_specifier>
        ed = nextToken();
        bool isINT = true;
        if (ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString().compare("int")){

        }
        else if (ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString().compare("char")){
            isINT = false;
        }else
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoType_specifier);
        auto err = init_declarator_list(isCONST, isINT, symbleTable);
        if (err.has_value())
            return err;
        return {};
    }
    //<声明_语句_列表> ::=
    //    <声明_语句>{','<声明_语句>}
    //<init_declarator_list> ::=
    //    <init_declarator>{','<init_declarator>}
    std::optional<CompilationError> Analyser::init_declarator_list(bool isCONST, bool isINT, SymbleTable *symbleTable) {
        auto err = init_declarator(isCONST, isINT, symbleTable);
        if (err.has_value())
            return err;
        auto ed = nextToken();
        while (ed.has_value() && ed.value().GetType() == TokenType::COMMA_SIGN){
            err = init_declarator(isCONST, isINT, symbleTable);
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
    std::optional<CompilationError> Analyser::init_declarator(bool isCONST, bool isINT, SymbleTable *symbleTable) {
        auto ed = nextToken();
        if (!ed.has_value() || ed.value().GetType() != TokenType::IDENTIFIER ){
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
        }
        auto name = ed.value();
        if(symbleTable->isDeclared(name.GetValueString()))
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
        ed = nextToken();
        _instructions.emplace_back(Operation::ipush, 0, 0);
        int32_t type = isINT? 1:2;
        int32_t kind = isCONST? 0:1;

        if(isCONST && !(ed.has_value() && ed.value().GetType() == TokenType::EQUAL_SIGN)){
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrConstantNeedValue);
        }
        if(ed.has_value() && ed.value().GetType() == TokenType::EQUAL_SIGN){
            if(!symbleTable->addSymble(name.GetValueString(),kind,type,1,-1))
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
            int index = symbleTable->index_in_all(name.GetValueString()).first;
            _instructions.emplace_back(Operation::ipush, index, 0);
            auto err = expression(isINT,symbleTable);
            if (err.has_value())
                return err;
            //存值
            if(isINT) {
                _instructions.emplace_back(Operation::istore, 0, 0);
            }
            else {
                _instructions.emplace_back(Operation::cstore, 0, 0);
            }
        }else {
            if(!symbleTable->addSymble(name.GetValueString(),kind,type,0,-1))
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
            unreadToken();
        }
        return {};
    }








    //<函数_定义> ::=
    //    <类型_说明符号><标识符> '(' [<参数_声明>{','<参数_声明>}] ')'<函数体_语句>
    //<function-definition> ::=
    //    <type-specifier><identifier>'(' [<parameter_declaration>{','<parameter_declaration>}] ')'<compound-statement>
    std::optional<CompilationError> Analyser::function_definition() {
        SymbleTable symbleTable = new SymbleTable(_symbleTable);
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
            if(_symbleTable.addSymble(name, 2,type,0,0))
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
        }
        else return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);

        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);
        ed = nextToken();
        if(ed.has_value()&& ed.value().GetType() == TokenType::RIGHT_BRACKET){
            return {};
        }
        unreadToken();
        int32_t count = 1;
        while(true){
            auto err = parameter_declaration(&symbleTable);
            if (err.has_value())
                return err;
            ed = nextToken();
            if(!ed.has_value())
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
            if(ed.has_value()&& ed.value().GetType() == TokenType::RIGHT_BRACKET) break;
            if(ed.has_value() && ed.value().GetType() != TokenType::COMMA_SIGN)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
            count++;
        }
        _symbleTable.changeSizeOfFun(name, count);

        auto err = compound_statement(&symbleTable);
        if (err.has_value())
            return err;
        return {};
    }

//<参数_声明> ::=  [<const_修饰符>]<类型_说明符号><标识符>
//<parameter_declaration> ::=  [<const_qualifier>]<type_specifier><identifier>
    std::optional<CompilationError> Analyser::parameter_declaration( SymbleTable *symbleTable) {
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
            if(symbleTable->addSymble(name, kind,type,0,0))
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
        }else return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);

        return {};
    }

//<函数体_语句> ::=  '{' {<变量_声明>} <语句_序列> '}'
//<compound_statement> ::=  '{' {<variable_declaration>} <statement_seq> '}'
    std::optional<CompilationError> Analyser::compound_statement( SymbleTable *symbleTable) {
        auto ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::BIG_LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);

        ed = nextToken();
        while(ed.has_value() && ed.value().GetType() == RESERVED_WORD &&
                (ed.value().GetValueString() == "const" ||
                        ed.value().GetValueString() == "int" ||
                        ed.value().GetValueString() == "char")){
            unreadToken();
            variable_declaration(symbleTable);
            ed = nextToken();
        }

        ed = nextToken();
        while(ed.has_value() && ed.value().GetType() != BIG_RIGHT_BRACKET ){
            unreadToken();
            statement(symbleTable);
            ed = nextToken();
        }
        if(!ed.has_value())
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
        return {};
    }

//<语句_序列> ::=
//	{<语句>}
//<statement_seq> ::=
//	{<statement>}
//<语句> ::=   <函数体_语句> |<条件_语句> |<loop_语句> |<jump_语句> |<print_语句> |<scan_语句> |<赋值_表达式>';' |<函数_call>';' |';'
//<statement> ::=   <compound_statement> |<condition_statement> |<loop_statement> |<jump_statement> |<print_statement> |<scan_statement> |<assignment_expression>';' |<function_call>';' |';'
    std::optional<CompilationError> Analyser::statement(SymbleTable *symbleTable) {
        //compound_statement 函数体_语句
        auto ed = nextToken();
        if(ed.has_value() && ed.value().GetType() == TokenType::BIG_LEFT_BRACKET){
            SymbleTable newsymbleTable = new SymbleTable(symbleTable);
            auto err = compound_statement(&newsymbleTable);
            if (err.has_value())
                return err;
            ed = nextToken();
            if(!ed.has_value() || ed.value().GetType() != TokenType::BIG_RIGHT_BRACKET){
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
            }
        }
        //赋值_表达式  assignment_expression 函数_call
        if(ed.has_value() && ed.value().GetType() == TokenType::IDENTIFIER) {
            std::string name = ed.value().GetValueString();
            int32_t kind = symbleTable->getKindByName(name);
            if(kind == 1){//赋值_表达式
                unreadToken();
                assignment_expression(symbleTable);
            }else if(kind == 2){//函数_call
                unreadToken();
                function_call(symbleTable);
            } else if(kind == 0)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrSetConst);
            else
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);

            ed = nextToken();
            if(!ed.has_value() || ed.value().GetType() != TokenType::SEMICOLON)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
        }
        if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "scan") {
            unreadToken();
            scan_statement(symbleTable);
        }
        if(ed.has_value() && ed.value().GetType() == TokenType::RESERVED_WORD && ed.value().GetValueString() == "print") {
            unreadToken();
            print_statement(symbleTable);
        }
        //todo:



        return {};
    }



//<print_语句> ::=  'print' '(' [<printable_列表>] ')' ';'
//<print_statement> ::=  'print' '(' [<printable_list>] ')' ';'
//<printable_列表>  ::=  <printable> {',' <printable>}
//<printable_list>  ::=  <printable> {',' <printable>}
    std::optional<CompilationError> Analyser::print_statement(SymbleTable *symbleTable) {
        auto ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::RESERVED_WORD || ed.value().GetValueString() != "scan") {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
        }
        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::LEFT_BRACKET) {
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);
        }

        auto err = printable(symbleTable);
        if(err.has_value())
            return err;

        ed = nextToken();
        while(ed.has_value() && ed.value().GetType() == TokenType::COMMA_SIGN){
            auto err = printable(symbleTable);
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
    //<printable> ::=  <表达式> | <string_串>
//<printable> ::=  <expression> | <string_literal>
    std::optional<CompilationError> Analyser::printable(SymbleTable *symbleTable) {
        auto ed = nextToken();
        if(ed.has_value() && ed.value().GetType() == TokenType::DOUBLE_QUOTES){

            ed = nextToken();
            if(!ed.has_value() || ed.value().GetType() != TokenType::STRING_SIGN){
                addString(ed.value().GetValueString());
                int32_t index = stringIndex(ed.value().GetValueString());
                if(index < 0)
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrEOF);
                _instructions.emplace_back(Operation::loadc, index, 0);
                _instructions.emplace_back(Operation::sprint, 0, 0);
            }


            ed = nextToken();
            if(!ed.has_value() || ed.value().GetType() != TokenType::STRING_SIGN)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedDoubleQuotes);
        }else{
            bool isint;
            auto err = print_expression(&isint,symbleTable);
            if(err.has_value())
                return err;
            if(isint){
                _instructions.emplace_back(Operation::iprint, 0, 0);
            }else{
                _instructions.emplace_back(Operation::cprint, 0, 0);
            }
        }
        return {};
    }
    std::optional<CompilationError> Analyser::print_expression(bool *isINT, SymbleTable *symbleTable) {
        auto err = additive_expression(isINT,symbleTable);
        if (err.has_value())
            return err;
        return {};
    }
    //<scan_语句> ::=  'scan' '(' <标识符> ')' ';'
//<scan_statement> ::=  'scan' '(' <identifier> ')' ';'
    std::optional<CompilationError> Analyser::scan_statement(SymbleTable *symbleTable) {
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
        one_symbol *symbol = symbleTable->getByName(name);
        int32_t kind = symbol->getKind();
        if(kind != 1)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);

        std::pair<int32_t ,int32_t > pii = symbleTable->index_in_all(name);
        _instructions.emplace_back(Operation::loada, pii.second, pii.first);

        int32_t type = symbol->getType();
        if(type == 1) {//int
            _instructions.emplace_back(Operation::iscan, 0, 0);
            _instructions.emplace_back(Operation::istore, 0, 0);
        }else if (type == 2) {//char
            _instructions.emplace_back(Operation::cscan, 0, 0);
            _instructions.emplace_back(Operation::cstore, 0, 0);
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
    }


    //<赋值_表达式> ::=  <标识符><赋值_操作符><表达式>
    //<assignment_expression> ::=  <identifier><assignment_operator><expression>
    std::optional<CompilationError> Analyser::assignment_expression(SymbleTable *symbleTable) {
        auto ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
        std::string name = ed.value().GetValueString();

        one_symbol * oneSymbol = symbleTable->getByName(name);
        if(oneSymbol->getKind() == -1)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
        int type = oneSymbol->getType();
        oneSymbol->setValue(1);

        // =
        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() == TokenType::EQUAL_SIGN)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoEqualSign);

        std::pair<int32_t ,int32_t > pii = symbleTable->index_in_all(name);
        //对栈操作
        _instructions.emplace_back(Operation::loada, pii.second, pii.first);

        auto err = expression(type == 1,symbleTable);
        if(err.has_value())
            return err;

        if(type == 1)
            _instructions.emplace_back(Operation::istore, 0, 0);
        else
            _instructions.emplace_back(Operation::cstore, 0, 0);

        return {};
    }




//<函数_call> ::=  <标识符> '(' [<表达式_列表>] ')'
//<function_call> ::=  <identifier> '(' [<expression_list>] ')'
//<表达式_列表> ::=  <表达式>{','<表达式>}
//<expression_list> ::=  <expression>{','<expression>}
    std::optional<CompilationError> Analyser::function_call(SymbleTable *symbleTable) {
//todo:
        auto ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() != TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNeedIdentifier);
        std::string name = ed.value().GetValueString();
        int32_t kind = symbleTable->getKindByName(name);
        if(kind != 2)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidVariableDeclaration);
        int32_t type = symbleTable->getTypeByName(name);
        if(type != 1 && type != 2)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidVariableDeclaration);
        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() !=TokenType::LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);
        bool isINT = type == 1;
        auto err = expression(isINT,symbleTable);
        if(err.has_value())
            return err;
        int32_t size = symbleTable->getSizeByName(name) -1 ;
        ed = nextToken();
        while(ed.has_value() && ed.value().GetType() == TokenType::COMMA_SIGN){
            auto err = expression(isINT,symbleTable);
            if(err.has_value())
                return err;
            ed = nextToken();
            size--;
        }
        if(size != 0)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrLessParams);


        ed = nextToken();
        if(!ed.has_value() || ed.value().GetType() !=TokenType::LEFT_BRACKET)
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoLeftBracket);
        return {};
    }





//<表达式> ::=  <加法_表达式>
//<expression> ::=  <additive_expression>
    std::optional<CompilationError> Analyser::expression(bool isINT, SymbleTable *symbleTable) {
        bool isintbegin = isINT;
        bool isint = isINT;
        auto err = additive_expression(&isint,symbleTable);
        if (err.has_value())
            return err;
        if(!isintbegin && isint)
            _instructions.emplace_back(Operation::i2c , 0, 0);
        return {};
    }
//<加法_表达式> ::=   <乘法_表达式>{<加法_操作符><乘法_表达式>}
//<additive_expression> ::=   <multiplicative_expression>{<additive_operator><multiplicative_expression>}
    std::optional<CompilationError> Analyser::additive_expression(bool *isINT, SymbleTable *symbleTable) {
        auto err = multiplicative_expression(isINT,symbleTable);
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
            err = multiplicative_expression(isINT,symbleTable);
            if (err.has_value())
                return err;

            if (type == TokenType::PLUS_SIGN)
                _instructions.emplace_back(Operation::iadd , 0, 0);
            else if (type == TokenType::MINUS_SIGN)
                _instructions.emplace_back(Operation::isub, 0, 0);
        }
        return {};
    }
//<乘法_表达式> ::=   <cast_表达式>{<乘法_操作符><cast_表达式>}
//<multiplicative_expression> ::=   <cast_expression>{<multiplicative_operator><cast_expression>}
    std::optional<CompilationError> Analyser::multiplicative_expression(bool *isINT, SymbleTable *symbleTable) {
        auto err = cast_expression(isINT,symbleTable);
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
            err = cast_expression(isINT,symbleTable);
            if (err.has_value())
                return err;

            if (type == TokenType::MULTIPLICATION_SIGN)
                _instructions.emplace_back(Operation::imul, 0, 0);
            else if (type == TokenType::DIVISION_SIGN)
                _instructions.emplace_back(Operation::idiv, 0, 0);
        }
        return {};
    }

//<cast_表达式> ::= {'('<类型_说明符号>')'}<单目_表达式>
//<cast_expression> ::= {'('<type_specifier>')'}<primary_expression>
    std::optional<CompilationError> Analyser::cast_expression(bool *isINT, SymbleTable *symbleTable) {
        auto next = nextToken();
        if(next.has_value() && next.value().GetType() == LEFT_BRACKET){
            next = nextToken();
            if(next.has_value() && next.value().GetType() == RESERVED_WORD && next.value().GetValueString()=="char"){

            }else if(next.has_value() && next.value().GetType() == RESERVED_WORD && next.value().GetValueString()=="int"){
                *isINT = true;
            }else return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoType_specifier);
            next = nextToken();
            if(!next.has_value() || next.value().GetType() != RIGHT_BRACKET)
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoRightBracket);
        }else{
            unreadToken();
        }
        auto err = primary_expression(isINT,symbleTable);
        if (err.has_value())
            return err;
        return {};
    }

//<primary_表达式> ::=   [<单目_操作符>]     '('<表达式>')'  |<标识符> |<整数_串> |<char_串> |<浮点数_串> |<函数_call>
//<primary_expression> ::=   [<unary_operator>]     '('<expression>')'  |<identifier> |<integer_literal> |<char_literal> |<floating_literal> |<function_call>
    std::optional<CompilationError> Analyser::primary_expression(bool *isINT, SymbleTable *symbleTable) {
// [<符号>]
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
            return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
        switch (next.value().GetType()) {
            //<标识符>
            case TokenType::IDENTIFIER: {
                std::string str = next.value().GetValueString();
                int32_t kind = symbleTable->getKindByName(str);
                if(kind == 0 || kind == 1){
                    std::pair<int32_t ,int32_t > pii = symbleTable->index_in_all(str);
                    if(pii.first == -1)
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotInitialized);
                    int32_t type = symbleTable->getTypeByName(str);
                    if(type == 1) {
                        *isINT = true;
                        _instructions.emplace_back(Operation::loada, pii.second,pii.first);//加载地址
                        _instructions.emplace_back(Operation::iload, 0,0);//得到数
                    }
                    else if(type == 2 ){//char
                        _instructions.emplace_back(Operation::loada, pii.second,pii.first);//加载地址
                        _instructions.emplace_back(Operation::cload, 0,0);//得到数
                        if(*isINT)
                            _instructions.emplace_back(Operation::c2i, 0,0);//得到数
                    }
                    else
                        return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
                    break;
                }else if(kind == 2){
                    //todo:
                    //<函数_call>
                    unreadToken();
                    auto err = function_call(symbleTable);
                    if(err.has_value())
                        return err;
                }


            }

                //<无符号整数>
            case TokenType::UNSIGNED_INTEGER: {
                *isINT = true;
                std::string numstr = next.value().GetValueString();
                std::stringstream myss;
                myss << numstr;
                int32_t num = 0;
                myss >> num;
                _instructions.emplace_back(Operation::ipush, num, 0);
                break;
            }
                //'('<表达式>')'
            case TokenType::LEFT_BRACKET: {
                if (!next.has_value())
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
                auto err = expression(isINT,symbleTable);
                if (err.has_value())
                    return err;
                next = nextToken();
                if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
                    return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
                break;
            }
            //<char_串>
            case TokenType::CHAR_SIGN:{
                std::string c = next.value().GetValueString();
                _instructions.emplace_back(Operation::cpush, c, 0);
                break;
            }




                // 这里和 <语句序列> 类似，需要根据预读结果调用不同的子程序
                // 但是要注意 default 返回的是一个编译错误
            default:
                return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
        }

        // 取负
        if(prefix == -1) {
            if(!*isINT)
                _instructions.emplace_back(Operation::c2i, 0,0);
            _instructions.emplace_back(Operation::ineg , 0, 0);
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
        if(stringIndex(string) >= 0){
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




}