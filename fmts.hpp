#include "fmt/core.h"
#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"

namespace fmt {
    template<>
    struct formatter<LNC0::ErrorCode> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template<typename FormatContext>
        auto format(const LNC0::ErrorCode &p, FormatContext &ctx) {
            std::string name;
            switch (p) {
                case LNC0::ErrNoError:
                    name = "No error.";
                    break;
                case LNC0::ErrStreamError:
                    name = "Stream error.";
                    break;
                case LNC0::ErrEOF:
                    name = "EOF";
                    break;
                case LNC0::ErrInvalidInput:
                    name = "The input is invalid.";
                    break;
                case LNC0::ErrInvalidIdentifier:
                    name = "Identifier is invalid";
                    break;
                case LNC0::ErrIntegerOverflow:
                    name = "The integer is too big(int32_t).";
                    break;
                case LNC0::ErrNoBegin:
                    name = "The program should start with 'begin'.";
                    break;
                case LNC0::ErrNoEnd:
                    name = "The program should end with 'end'.";
                    break;
                case LNC0::ErrNeedIdentifier:
                    name = "Need an identifier here.";
                    break;
                case LNC0::ErrConstantNeedValue:
                    name = "The constant need a value to initialize.";
                    break;
                case LNC0::ErrNoSemicolon:
                    name = "Zai? Wei shen me bu xie fen hao.";
                    break;
                case LNC0::ErrInvalidVariableDeclaration:
                    name = "The declaration is invalid.";
                    break;
                case LNC0::ErrIncompleteExpression:
                    name = "The expression is incomplete.";
                    break;
                case LNC0::ErrNotDeclared:
                    name = "The variable or constant must be declared before being used.";
                    break;
                case LNC0::ErrAssignToConstant:
                    name = "Trying to assign value to a constant.";
                    break;
                case LNC0::ErrDuplicateDeclaration:
                    name = "The identifier has been declared.";
                    break;
                case LNC0::ErrNotInitialized:
                    name = "The variable has not been initialized.";
                    break;
                case LNC0::ErrInvalidAssignment:
                    name = "The assignment statement is invalid.";
                    break;
                case LNC0::ErrInvalidPrint:
                    name = "The output statement is invalid.";
                    break;
                case LNC0::ErrNoEqualSign:
                    name = "There should be an Equal sign.";
                    break;
                case LNC0::ErrNoLeftBracket:
                    name = "There should be an LeftBracket sign.";
                    break;
                case LNC0::ErrNoRightBracket:
                    name = "There should be an RightBracket sign.";
                    break;
                case LNC0::ErrInvalidConstantExpression:
                    name = "The assignment Constant Expression is invalid.";
                    break;
                case LNC0::ErrSetConst:
                    name = "You can not set value to Const.";
                    break;
                case LNC0::ErrNoType_specifier:
                    name = "There should be a Type_specifier.";
                    break;
                case LNC0::ErrNeedRightMultipicationSign:
                    name = "There should be a \" *\\ \" .";
                    break;
                case LNC0::ErrNeedDoubleQuotes:
                    name = "There should be a \" .";
                    break;
                case LNC0::ErrNeedSingalQuote:
                    name = "There should be a \' .";
                    break;
                case LNC0::ErrLessParams:
                    name = "Need more Params.";
                    break;
                case LNC0::ErrVoidFun:
                    name = "Can not operate void function.";
                    break;
                case LNC0::ErrNeedMain:
                    name = "There must be a main function.";
                    break;
                case LNC0::ErrBreakNoneLoop:
                    name = "There is no loop to break.";
                    break;
                case LNC0::ErrContinueNoneLoop:
                    name = "There is no loop to continue.";
                    break;

            }
            return format_to(ctx.out(), name);
        }
    };

    template<>
    struct formatter<LNC0::CompilationError> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template<typename FormatContext>
        auto format(const LNC0::CompilationError &p, FormatContext &ctx) {
            return format_to(ctx.out(), "Line: {} Column: {} Error: {}", p.GetPos().first, p.GetPos().second,
                             p.GetCode());
        }
    };
}

namespace fmt {
    template<>
    struct formatter<LNC0::Token> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template<typename FormatContext>
        auto format(const LNC0::Token &p, FormatContext &ctx) {
            return format_to(ctx.out(),
                             "Line: {} Column: {} Type: {} Value: {}",
                             p.GetStartPos().first, p.GetStartPos().second, p.GetType(), p.GetValueString());
        }
    };

    template<>
    struct formatter<LNC0::TokenType> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template<typename FormatContext>
        auto format(const LNC0::TokenType &p, FormatContext &ctx) {
            std::string name;
            switch (p) {
                case LNC0::NULL_TOKEN:
                    name = "NullToken";
                    break;
                case LNC0::UNSIGNED_INTEGER:
                    name = "UnsignedInteger";
                    break;
                case LNC0::IDENTIFIER:
                    name = "Identifier";
                    break;
                case LNC0::BEGIN:
                    name = "Begin";
                    break;
                case LNC0::END:
                    name = "End";
                    break;
                case LNC0::VAR:
                    name = "Var";
                    break;
                case LNC0::CONST:
                    name = "Const";
                    break;
                case LNC0::PRINT:
                    name = "Print";
                    break;
                case LNC0::PLUS_SIGN:
                    name = "PlusSign";
                    break;
                case LNC0::MINUS_SIGN:
                    name = "MinusSign";
                    break;
                case LNC0::MULTIPLICATION_SIGN:
                    name = "MultiplicationSign";
                    break;
                case LNC0::DIVISION_SIGN:
                    name = "DivisionSign";
                    break;
                case LNC0::EQUAL_SIGN:
                    name = "EqualSign";
                    break;
                case LNC0::SEMICOLON:
                    name = "Semicolon";
                    break;
                case LNC0::LEFT_BRACKET:
                    name = "LeftBracket";
                    break;
                case LNC0::RIGHT_BRACKET:
                    name = "RightBracket";
                    break;
                case LNC0::DOUBLE_EQUAL_SIGN:
                    name = "DoubleEqual";
                    break;
                case LNC0::LESS_EQUAL_SIGN:
                    name = "LessEqual";
                    break;
                case LNC0::GREATER_EQUAL_SIGN:
                    name = "GreaterEqual";
                    break;
                case LNC0::LESS_SIGN:
                    name = "Less";
                    break;
                case LNC0::GREATER_SIGN:
                    name = "Greater";
                    break;
                case LNC0::UNEQUAL_SIGN:
                    name = "Unequal";
                    break;
                case LNC0::SIXTEEN_INTEGER:
                    name = "SixteenInteger";
                    break;
                case LNC0::RESERVED_WORD:
                    name = "ReservedWord";
                    break;
                case LNC0::DOUBLE_QUOTES:
                    name = "DoubleQuotes";
                    break;
                case LNC0::SINGLE_QUOTE:
                    name = "SingleQuote";
                    break;
                case LNC0::BIG_LEFT_BRACKET:
                    name = "BigLeftBracket";
                    break;
                case LNC0::BIG_RIGHT_BRACKET:
                    name = "BigRightBracket";
                    break;
                case LNC0::COMMA_SIGN:
                    name = "CommaSign";
                    break;
                case LNC0::SINGLE_LINE_COMMENT_SIGN:
                    name = "SingleLineComment";
                    break;
                case LNC0::MULTI_LINE_COMMENT_RIGHT:
                    name = "MultiLineCommentRight";
                    break;
                case LNC0::MULTI_LINE_COMMENT_LEFT:
                    name = "MultiLineCommentLeft";
                    break;
                case LNC0::STRING_SIGN:
                    name = "String";
                    break;
                case LNC0::CHAR_SIGN:
                    name = "Char";
                    break;

            }
            return format_to(ctx.out(), name);
        }
    };
}

namespace fmt {
    template<>
    struct formatter<LNC0::Operation> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template<typename FormatContext>
        auto format(const LNC0::Operation &p, FormatContext &ctx) {
            std::string name;
            switch (p) {
                case LNC0::loada:
                    name = "loada";
                    break;
                case LNC0::ipush:
                    name = "ipush";
                    break;
                case LNC0::istore:
                    name = "istore";
                    break;
                case LNC0::iadd:
                    name = "iadd";
                    break;
                case LNC0::isub:
                    name = "isub";
                    break;
                case LNC0::imul:
                    name = "imul";
                    break;
                case LNC0::idiv:
                    name = "idiv";
                    break;
                case LNC0::ineg:
                    name = "ineg";
                    break;
                case LNC0::i2c:
                    name = "i2c";
                    break;
                case LNC0::iload:
                    name = "iload";
                    break;
                case LNC0::jmp:
                    name = "jmp";
                    break;
                case LNC0::bipush:
                    name = "bipush";
                    break;
                case LNC0::iscan:
                    name = "iscan";
                    break;
                case LNC0::cscan:
                    name = "cscan";
                    break;
                case LNC0::iprint:
                    name = "iprint";
                    break;
                case LNC0::cprint:
                    name = "cprint";
                    break;
                case LNC0::sprint:
                    name = "sprint";
                    break;
                case LNC0::loadc:
                    name = "loadc";
                    break;
                case LNC0::ret:
                    name = "ret";
                    break;
                case LNC0::iret:
                    name = "iret";
                    break;
                case LNC0::call:
                    name = "call";
                    break;
                case LNC0::_F:
                    name = ".F";
                    break;
                case LNC0::_start:
                    name = ".start";
                    break;
                case LNC0::je:
                    name = "je";
                    break;
                case LNC0::jne:
                    name = "jne";
                    break;
                case LNC0::jl:
                    name = "jl";
                    break;
                case LNC0::jle:
                    name = "jle";
                    break;
                case LNC0::jg:
                    name = "jg";
                    break;
                case LNC0::jge:
                    name = "jge";
                    break;
                case LNC0::nop:
                    name = "nop";
                    break;
            }
            return format_to(ctx.out(), name);
        }
    };

    template<>
    struct formatter<LNC0::Instruction> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template<typename FormatContext>
        auto format(const LNC0::Instruction &p, FormatContext &ctx) {
            std::string name;
            switch (p.GetOperation()) {
                case LNC0::iadd:
                case LNC0::istore:
                case LNC0::isub:
                case LNC0::imul:
                case LNC0::idiv:
                case LNC0::ineg:
                case LNC0::i2c:
                case LNC0::iload:
                case LNC0::iscan:
                case LNC0::cscan:
                case LNC0::iprint:
                case LNC0::cprint:
                case LNC0::sprint:
                case LNC0::ret:
                case LNC0::iret:
                case LNC0::nop:
                    return format_to(ctx.out(), "{} {}", p.GetCount()-1, p.GetOperation());
                case LNC0::ipush:
                case LNC0::bipush:
                case LNC0::jmp:
                case LNC0::loadc:
                case LNC0::call:
                case LNC0::je:
                case LNC0::jne:
                case LNC0::jle:
                case LNC0::jl:
                case LNC0::jg:
                case LNC0::jge:
                    return format_to(ctx.out(), "{} {} {}", p.GetCount()-1, p.GetOperation(), p.GetX());

                case LNC0::loada:
                    return format_to(ctx.out(), "{} {} {},{}", p.GetCount()-1, p.GetOperation(), p.GetX(), p.GetY());

                case LNC0::_F:
                    return format_to(ctx.out(), "{}{}:", p.GetOperation(), p.GetX());

                case LNC0::_start:
                    return format_to(ctx.out(), "{}:", p.GetOperation());
            }
            return format_to(ctx.out(), "ILL");
        }
    };
}