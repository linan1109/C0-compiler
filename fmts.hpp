#include "fmt/core.h"
#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"

namespace fmt {
    template<>
    struct formatter<miniplc0::ErrorCode> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template<typename FormatContext>
        auto format(const miniplc0::ErrorCode &p, FormatContext &ctx) {
            std::string name;
            switch (p) {
                case miniplc0::ErrNoError:
                    name = "No error.";
                    break;
                case miniplc0::ErrStreamError:
                    name = "Stream error.";
                    break;
                case miniplc0::ErrEOF:
                    name = "EOF";
                    break;
                case miniplc0::ErrInvalidInput:
                    name = "The input is invalid.";
                    break;
                case miniplc0::ErrInvalidIdentifier:
                    name = "Identifier is invalid";
                    break;
                case miniplc0::ErrIntegerOverflow:
                    name = "The integer is too big(int64_t).";
                    break;
                case miniplc0::ErrNoBegin:
                    name = "The program should start with 'begin'.";
                    break;
                case miniplc0::ErrNoEnd:
                    name = "The program should end with 'end'.";
                    break;
                case miniplc0::ErrNeedIdentifier:
                    name = "Need an identifier here.";
                    break;
                case miniplc0::ErrConstantNeedValue:
                    name = "The constant need a value to initialize.";
                    break;
                case miniplc0::ErrNoSemicolon:
                    name = "Zai? Wei shen me bu xie fen hao.";
                    break;
                case miniplc0::ErrInvalidVariableDeclaration:
                    name = "The declaration is invalid.";
                    break;
                case miniplc0::ErrIncompleteExpression:
                    name = "The expression is incomplete.";
                    break;
                case miniplc0::ErrNotDeclared:
                    name = "The variable or constant must be declared before being used.";
                    break;
                case miniplc0::ErrAssignToConstant:
                    name = "Trying to assign value to a constant.";
                    break;
                case miniplc0::ErrDuplicateDeclaration:
                    name = "The variable or constant has been declared.";
                    break;
                case miniplc0::ErrNotInitialized:
                    name = "The variable has not been initialized.";
                    break;
                case miniplc0::ErrInvalidAssignment:
                    name = "The assignment statement is invalid.";
                    break;
                case miniplc0::ErrInvalidPrint:
                    name = "The output statement is invalid.";
                    break;
                case miniplc0::ErrNoEqualSign:
                    name = "There should be an Equal sign.";
                    break;
                case miniplc0::ErrNoLeftBracket:
                    name = "There should be an LeftBracket sign.";
                    break;
                case miniplc0::ErrNoRightBracket:
                    name = "There should be an RightBracket sign.";
                    break;
                case miniplc0::ErrInvalidConstantExpression:
                    name = "The assignment Constant Expression is invalid.";
                    break;
                case miniplc0::ErrSetConst:
                    name = "You can not set value to Const.";
                    break;
                case miniplc0::ErrNoType_specifier:
                    name = "There should be a Type_specifier.";
                    break;
                case miniplc0::ErrNeedRightMultipicationSign:
                    name = "There should be a \" *\\ \" .";
                    break;
                case miniplc0::ErrNeedDoubleQuotes:
                    name = "There should be a \" .";
                    break;
                case miniplc0::ErrNeedSingalQuote:
                    name = "There should be a \' .";
                    break;
                case miniplc0::ErrLessParams:
                    name = "Need more Params.";
                    break;
            }
            return format_to(ctx.out(), name);
        }
    };

    template<>
    struct formatter<miniplc0::CompilationError> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template<typename FormatContext>
        auto format(const miniplc0::CompilationError &p, FormatContext &ctx) {
            return format_to(ctx.out(), "Line: {} Column: {} Error: {}", p.GetPos().first, p.GetPos().second,
                             p.GetCode());
        }
    };
}

namespace fmt {
    template<>
    struct formatter<miniplc0::Token> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template<typename FormatContext>
        auto format(const miniplc0::Token &p, FormatContext &ctx) {
            return format_to(ctx.out(),
                             "Line: {} Column: {} Type: {} Value: {}",
                             p.GetStartPos().first, p.GetStartPos().second, p.GetType(), p.GetValueString());
        }
    };

    template<>
    struct formatter<miniplc0::TokenType> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template<typename FormatContext>
        auto format(const miniplc0::TokenType &p, FormatContext &ctx) {
            std::string name;
            switch (p) {
                case miniplc0::NULL_TOKEN:
                    name = "NullToken";
                    break;
                case miniplc0::UNSIGNED_INTEGER:
                    name = "UnsignedInteger";
                    break;
                case miniplc0::IDENTIFIER:
                    name = "Identifier";
                    break;
                case miniplc0::BEGIN:
                    name = "Begin";
                    break;
                case miniplc0::END:
                    name = "End";
                    break;
                case miniplc0::VAR:
                    name = "Var";
                    break;
                case miniplc0::CONST:
                    name = "Const";
                    break;
                case miniplc0::PRINT:
                    name = "Print";
                    break;
                case miniplc0::PLUS_SIGN:
                    name = "PlusSign";
                    break;
                case miniplc0::MINUS_SIGN:
                    name = "MinusSign";
                    break;
                case miniplc0::MULTIPLICATION_SIGN:
                    name = "MultiplicationSign";
                    break;
                case miniplc0::DIVISION_SIGN:
                    name = "DivisionSign";
                    break;
                case miniplc0::EQUAL_SIGN:
                    name = "EqualSign";
                    break;
                case miniplc0::SEMICOLON:
                    name = "Semicolon";
                    break;
                case miniplc0::LEFT_BRACKET:
                    name = "LeftBracket";
                    break;
                case miniplc0::RIGHT_BRACKET:
                    name = "RightBracket";
                    break;
                case miniplc0::DOUBLE_EQUAL_SIGN:
                    name = "DoubleEqual";
                    break;
                case miniplc0::LESS_EQUAL_SIGN:
                    name = "LessEqual";
                    break;
                case miniplc0::GREATER_EQUAL_SIGN:
                    name = "GreaterEqual";
                    break;
                case miniplc0::LESS_SIGN:
                    name = "Less";
                    break;
                case miniplc0::GREATER_SIGN:
                    name = "Greater";
                    break;
                case miniplc0::UNEQUAL_SIGN:
                    name = "Unequal";
                    break;
                case miniplc0::SIXTEEN_INTEGER:
                    name = "SixteenInteger";
                    break;
                case miniplc0::RESERVED_WORD:
                    name = "ReservedWord";
                    break;
                case miniplc0::DOUBLE_QUOTES:
                    name = "DoubleQuotes";
                    break;
                case miniplc0::SINGLE_QUOTE:
                    name = "SingleQuote";
                    break;
                case miniplc0::BIG_LEFT_BRACKET:
                    name = "BigLeftBracket";
                    break;
                case miniplc0::BIG_RIGHT_BRACKET:
                    name = "BigRightBracket";
                    break;
                case miniplc0::COMMA_SIGN:
                    name = "CommaSign";
                    break;
                case miniplc0::SINGLE_LINE_COMMENT_SIGN:
                    name = "SingleLineComment";
                    break;
                case miniplc0::MULTI_LINE_COMMENT_RIGHT:
                    name = "MultiLineCommentRight";
                    break;
                case miniplc0::MULTI_LINE_COMMENT_LEFT:
                    name = "MultiLineCommentLeft";
                    break;
                case miniplc0::STRING_SIGN:
                    name = "String";
                    break;
                case miniplc0::CHAR_SIGN:
                    name = "Char";
                    break;

            }
            return format_to(ctx.out(), name);
        }
    };
}

namespace fmt {
    template<>
    struct formatter<miniplc0::Operation> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template<typename FormatContext>
        auto format(const miniplc0::Operation &p, FormatContext &ctx) {
            std::string name;
            switch (p) {
                case miniplc0::loada:
                    name = "loada";
                    break;
                case miniplc0::ipush:
                    name = "ipush";
                    break;
                case miniplc0::istore:
                    name = "istore";
                    break;
                case miniplc0::cstore:
                    name = "cstore";
                    break;
                case miniplc0::iadd:
                    name = "iadd";
                    break;
                case miniplc0::isub:
                    name = "isub";
                    break;
                case miniplc0::imul:
                    name = "imul";
                    break;
                case miniplc0::idiv:
                    name = "idiv";
                    break;
                case miniplc0::ineg:
                    name = "ineg";
                    break;
                case miniplc0::i2c:
                    name = "i2c";
                    break;
                case miniplc0::iload:
                    name = "iload";
                    break;
                case miniplc0::jmp:
                    name = "jmp";
                    break;
                case miniplc0::cpush:
                    name = "cpush";
                    break;
                case miniplc0::iscan:
                    name = "iscan";
                    break;
                case miniplc0::cscan:
                    name = "cscan";
                    break;
                case miniplc0::iprint:
                    name = "iprint";
                    break;
                case miniplc0::cprint:
                    name = "cprint";
                    break;
                case miniplc0::sprint:
                    name = "sprint";
                    break;
                case miniplc0::loadc:
                    name = "loadc";
                    break;

                case miniplc0::ret:
                    name = "ret";
                    break;
                case miniplc0::iret:
                    name = "iret";
                    break;
                case miniplc0::cret:
                    name = "cret";
                    break;
                case miniplc0::call:
                    name = "call";
                    break;
                case miniplc0::_F:
                    name = ".F";
                    break;
                case miniplc0::_start:
                    name = ".start";
                    break;
            }
            return format_to(ctx.out(), name);
        }
    };

    template<>
    struct formatter<miniplc0::Instruction> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template<typename FormatContext>
        auto format(const miniplc0::Instruction &p, FormatContext &ctx) {
            std::string name;
            switch (p.GetOperation()) {
                case miniplc0::iadd:
                case miniplc0::istore:
                case miniplc0::cstore:
                case miniplc0::isub:
                case miniplc0::imul:
                case miniplc0::idiv:
                case miniplc0::ineg:
                case miniplc0::i2c:
                case miniplc0::iload:
                case miniplc0::iscan:
                case miniplc0::cscan:
                case miniplc0::iprint:
                case miniplc0::cprint:
                case miniplc0::sprint:
                case miniplc0::ret:
                case miniplc0::iret:
                case miniplc0::cret:
                    return format_to(ctx.out(), "{} {}", p.GetCount(), p.GetOperation());
                case miniplc0::ipush:
                case miniplc0::cpush:
                case miniplc0::jmp:
                case miniplc0::loadc:
                case miniplc0::call:
                    return format_to(ctx.out(), "{} {} {}", p.GetCount(), p.GetOperation(), p.GetX());

                case miniplc0::loada:
                    return format_to(ctx.out(), "{} {} {},{}", p.GetCount(), p.GetOperation(), p.GetX(), p.GetY());

                case miniplc0::_F:
                    return format_to(ctx.out(), "{}{}:", p.GetOperation(), p.GetX());

                case miniplc0::_start:
                    return format_to(ctx.out(), "{}:", p.GetOperation());
            }
            return format_to(ctx.out(), "ILL");
        }
    };
}