#include "tokenizer/tokenizer.h"

#include <cctype>
#include <sstream>
#include <cstring>

namespace LNC0 {

    std::pair<std::optional<Token>, std::optional<CompilationError>> Tokenizer::NextToken() {
        if (!_initialized)
            readAll();
        if (_rdr.bad())
            return std::make_pair(std::optional<Token>(),
                                  std::make_optional<CompilationError>(0, 0, ErrorCode::ErrStreamError));
        if (isEOF())
            return std::make_pair(std::optional<Token>(),
                                  std::make_optional<CompilationError>(0, 0, ErrorCode::ErrEOF));
        auto p = nextToken();
        if (p.second.has_value()) return std::make_pair(p.first, p.second);

        auto err = checkToken(p.first.value());
        if (err.has_value())
            return std::make_pair(p.first, err.value());
        return std::make_pair(p.first, std::optional<CompilationError>());

    }

        std::pair<std::vector<Token>, std::optional<CompilationError>> Tokenizer::AllTokens() {
            std::vector<Token> result;
            while (true) {
                auto p = NextToken();
                if (p.second.has_value()) {
                    if (p.second.value().GetCode() == ErrorCode::ErrEOF)
                        return std::make_pair(result, std::optional<CompilationError>());
                    else
                        return std::make_pair(std::vector<Token>(), p.second);
                }
                result.emplace_back(p.first.value());
            }
        }

        // 注意：这里的返回值中 Token 和 CompilationError 只能返回一个，不能同时返回。
        std::pair<std::optional<Token>, std::optional<CompilationError>> Tokenizer::nextToken() {
            // 用于存储已经读到的组成当前token字符
            std::stringstream ss;
            // 分析token的结果，作为此函数的返回值
            std::pair<std::optional<Token>, std::optional<CompilationError>> result;
            // <行号，列号>，表示当前token的第一个字符在源代码中的位置
            std::pair<int64_t, int64_t> pos;
            // 记录当前自动机的状态，进入此函数时是初始状态
            DFAState current_state = DFAState::INITIAL_STATE;
            // 这是一个死循环，除非主动跳出
            // 每一次执行while内的代码，都可能导致状态的变更
            while (true) {
                // 读一个字符，请注意auto推导得出的类型是std::optional<char>
                // 这里其实有两种写法
                // 1. 每次循环前立即读入一个 char
                // 2. 只有在可能会转移的状态读入一个 char
                // 因为我们实现了 unread，为了省事我们选择第一种
                auto current_char = nextChar();
                // 针对当前的状态进行不同的操作
                switch (current_state) {

                    // 初始状态
                    // 这个 case 我们给出了核心逻辑，但是后面的 case 不用照搬。
                    case INITIAL_STATE: {
                        // 已经读到了文件尾
                        if (!current_char.has_value())
                            // 返回一个空的token，和编译错误ErrEOF：遇到了文件尾
                            return std::make_pair(std::optional<Token>(),
                                                  std::make_optional<CompilationError>(0, 0, ErrEOF));

                        // 获取读到的字符的值，注意auto推导出的类型是char
                        auto ch = current_char.value();
                        // 标记是否读到了不合法的字符，初始化为否
                        auto invalid = false;

                        // 使用了自己封装的判断字符类型的函数，定义于 tokenizer/utils.hpp
                        // see https://en.cppreference.com/w/cpp/string/byte/isblank
                        if (LNC0::isspace(ch)) // 读到的字符是空白字符（空格、换行、制表符等）
                            current_state = DFAState::INITIAL_STATE; // 保留当前状态为初始状态，此处直接break也是可以的
                        else if (!LNC0::isprint(ch)) // control codes and backspace
                            invalid = true;
                        else if (LNC0::isdigit(ch)) // 读到的字符是数字
                            current_state = DFAState::UNSIGNED_INTEGER_STATE; // 切换到无符号整数的状态
                        else if (LNC0::isalpha(ch)) // 读到的字符是英文字母
                            current_state = DFAState::IDENTIFIER_STATE; // 切换到标识符的状态
                        else {
                            switch (ch) {
                                case '=': // 如果读到的字符是`=`，则切换到等于号的状态
                                    current_state = DFAState::EQUAL_SIGN_STATE;
                                    break;
                                case '-':
                                    // 请填空：切换到减号的状态
                                    current_state = DFAState::MINUS_SIGN_STATE;
                                    break;
                                case '+':
                                    // 请填空：切换到加号的状态
                                    current_state = DFAState::PLUS_SIGN_STATE;
                                    break;
                                case '*':
                                    // 请填空：*
                                    current_state = DFAState::MULTIPLICATION_SIGN_STATE;
                                    break;
                                case '/':
                                    // 请填空：/
                                    current_state = DFAState::DIVISION_SIGN_STATE;
                                    break;
                                case ';':
                                    // 请填空：;
                                    current_state = DFAState::SEMICOLON_STATE;
                                    break;
                                case '(':
                                    // 请填空：(
                                    current_state = DFAState::LEFTBRACKET_STATE;
                                    break;
                                case ')':
                                    // 请填空：)
                                    current_state = DFAState::RIGHTBRACKET_STATE;
                                    break;
                                case '<':
                                    // 请填空：)
                                    current_state = DFAState::LESS_STATE;
                                    break;
                                case '>':
                                    // 请填空：)
                                    current_state = DFAState::GREATER_STATE;
                                    break;
                                case '!':
                                    // 请填空：)
                                    current_state = DFAState::EXCLAMATION_STATE;
                                    break;
                                case '\"':
                                    // 请填空：)
                                    current_state = DFAState::DOUBLE_QUOTES_STATE;
                                    break;
                                case '\'':
                                    // 请填空：)
                                    current_state = DFAState::SINGLE_QUOTE_STATE;
                                    break;
                                case '{':
                                    // 请填空：(
                                    current_state = DFAState::BIG_LEFTBRACKET_STATE;
                                    break;
                                case '}':
                                    // 请填空：)
                                    current_state = DFAState::BIG_RIGHTBRACKET_STATE;
                                    break;
                                case ',':
                                    // 请填空：)
                                    current_state = DFAState::COMMA_STATE;
                                    break;
                                    ///// 请填空：
                                    ///// 对于其他的可接受字符
                                    ///// 切换到对应的状态

                                    // 不接受的字符导致的不合法的状态
                                default:
                                    invalid = true;
                                    break;
                            }
                        }
                        // 如果读到的字符导致了状态的转移，说明它是一个token的第一个字符
                        if (current_state != DFAState::INITIAL_STATE)
                            pos = previousPos(); // 记录该字符的的位置为token的开始位置
                        // 读到了不合法的字符
                        if (invalid) {
                            // 回退这个字符
                            unreadLast();
                            // 返回编译错误：非法的输入
                            return std::make_pair(std::optional<Token>(),
                                                  std::make_optional<CompilationError>(pos,
                                                                                       ErrorCode::ErrInvalidInput));
                        }
                        // 如果读到的字符导致了状态的转移，说明它是一个token的第一个字符
                        if (current_state != DFAState::INITIAL_STATE) // ignore white spaces
                            ss << ch; // 存储读到的字符
                        break;
                    }


                        // 当前状态是无符号整数
                    case UNSIGNED_INTEGER_STATE: {
                        // 请填空：
                        // 如果当前已经读到了文件尾，则解析已经读到的字符串为整数
                        if (!current_char.has_value()) {
                            char *chxx = new char[100];
                            ss >> chxx;
                            if (chxx[0] == '0' && (chxx[1] >= '0' && chxx[1] <= '9')) {
                                return std::make_pair(std::optional<Token>(),
                                                      std::make_optional<CompilationError>(pos,
                                                                                           ErrorCode::ErrInvalidInput));
                            }
                            else if (chxx[0] == '0' && (chxx[1] == 'x' || chxx[1] == 'X') && std::strlen(chxx) > 2) {
                                for (long long unsigned int i = 2; i < std::strlen(chxx); i++)
                                    if (!(chxx[i] >= '0' && chxx[i] <= '9') && !(chxx[i] >= 'a' && chxx[i] <= 'f') &&
                                        !(chxx[i] >= 'A' && chxx[i] <= 'F'))
                                        return std::make_pair(std::optional<Token>(),
                                                              std::make_optional<CompilationError>(pos,
                                                                                                   ErrorCode::ErrInvalidInput));
                                unsigned int lngHex;
                                sscanf(chxx + 2, "%x", &lngHex);
                                if(lngHex > 2147483647)
                                    return std::make_pair(std::optional<Token>(),
                                                          std::make_optional<CompilationError>(pos,
                                                                                               ErrorCode::ErrIntegerOverflow));
                                std::string str = std::to_string(lngHex);
                                return std::make_pair(
                                        std::make_optional<Token>(TokenType::UNSIGNED_INTEGER, str, pos, currentPos()),
                                        std::optional<CompilationError>());
                            } else {
                                for (long long unsigned int i = 0; i < std::strlen(chxx); i++)
                                    if (!(chxx[i] >= '0' && chxx[i] <= '9'))
                                        return std::make_pair(std::optional<Token>(),
                                                              std::make_optional<CompilationError>(pos,
                                                                                                   ErrorCode::ErrInvalidInput));
                                long long int lngHex;
                                sscanf(chxx, "%lld", &lngHex);
                                if(lngHex > 2147483647)
                                    return std::make_pair(std::optional<Token>(),
                                                          std::make_optional<CompilationError>(pos,
                                                                                               ErrorCode::ErrIntegerOverflow));
                                std::string str = std::to_string(lngHex);

                                return std::make_pair(
                                        std::make_optional<Token>(TokenType::UNSIGNED_INTEGER, str, pos, currentPos()),
                                        std::optional<CompilationError>());
                            }
                        }
                        //     解析成功则返回无符号整数类型的token，否则返回编译错误
                        auto ch = current_char.value();
                        // 如果读到的字符是数字，则存储读到的字符
                        while (current_char.has_value() && (LNC0::isdigit(ch) || LNC0::isalpha(ch))) {
                            ss << ch;
                            current_char = nextChar();
                            ch = current_char.value();
                        }
                        //unreadLast();
                        // 如果读到的是字母，则存储读到的字符，并切换状态到标识符
                        // 如果读到的字符不是上述情况之一，则回退读到的字符，并解析已经读到的字符串为整数
                        if (current_char.has_value())unreadLast();
                        char *chxx = new char[100];
                        ss >> chxx;
                        if (chxx[0] == '0' && (chxx[1] >= '0' && chxx[1] <= '9')) {
                            return std::make_pair(std::optional<Token>(),
                                                  std::make_optional<CompilationError>(pos,
                                                                                       ErrorCode::ErrInvalidInput));
                        }
                        else if (chxx[0] == '0' && (chxx[1] == 'x' || chxx[1] == 'X')&& std::strlen(chxx) > 2) {
                            for (long long unsigned int i = 2; i < std::strlen(chxx); i++)
                                if (!(chxx[i] >= '0' && chxx[i] <= '9') && !(chxx[i] >= 'a' && chxx[i] <= 'f') &&
                                    !(chxx[i] >= 'A' && chxx[i] <= 'F'))
                                    return std::make_pair(std::optional<Token>(),
                                                          std::make_optional<CompilationError>(pos,
                                                                                               ErrorCode::ErrInvalidInput));
                            unsigned int lngHex;
                            sscanf(chxx, "%x", &lngHex);
                            if(lngHex > 2147483647)
                                return std::make_pair(std::optional<Token>(),
                                                      std::make_optional<CompilationError>(pos,
                                                                                           ErrorCode::ErrIntegerOverflow));
                            std::string str = std::to_string(lngHex);
                            return std::make_pair(
                                    std::make_optional<Token>(TokenType::UNSIGNED_INTEGER, str, pos, currentPos()),
                                    std::optional<CompilationError>());
                        } else {
                            for (long long unsigned int i = 0; i < std::strlen(chxx); i++)
                                if (!(chxx[i] >= '0' && chxx[i] <= '9'))
                                    return std::make_pair(std::optional<Token>(),
                                                          std::make_optional<CompilationError>(pos,
                                                                                               ErrorCode::ErrInvalidInput));
                            long long int lngHex;
                            sscanf(chxx, "%lld", &lngHex);
                            if(lngHex > 2147483647)
                                return std::make_pair(std::optional<Token>(),
                                                      std::make_optional<CompilationError>(pos,
                                                                                           ErrorCode::ErrIntegerOverflow));
                            std::string str = std::to_string(lngHex);
                            return std::make_pair(
                                    std::make_optional<Token>(TokenType::UNSIGNED_INTEGER, str, pos, currentPos()),
                                    std::optional<CompilationError>());
                        }

                        //     解析成功则返回无符号整数类型的token，否则返回编译错误
                        break;
                    }
                    case IDENTIFIER_STATE: {
                        // 请填空：
                        std::string str;
                        // 如果当前已经读到了文件尾，则解析已经读到的字符串
                        if (!current_char.has_value()) {
                            ss >> str;
                            //     如果解析结果是关键字，那么返回对应关键字的token，否则返回标识符的token
                            if (isReservedWord(str))
                                return std::make_pair(
                                        std::make_optional<Token>(TokenType::RESERVED_WORD, str, pos, currentPos()),
                                        std::optional<CompilationError>());
                            return std::make_pair(
                                    std::make_optional<Token>(TokenType::IDENTIFIER, str, pos, currentPos()),
                                    std::optional<CompilationError>());
                        }
                        // 如果读到的是字符或字母，则存储读到的字符
                        auto ch = current_char.value();
                        if (LNC0::isdigit(ch) || LNC0::isalpha(ch))
                            ss << ch;
                            // 如果读到的字符不是上述情况之一，则回退读到的字符，并解析已经读到的字符串
                        else {
                            //     如果解析结果是关键字，那么返回对应关键字的token，否则返回标识符的token
                            unreadLast();
                            ss >> str;
                            if (isReservedWord(str))
                                return std::make_pair(
                                        std::make_optional<Token>(TokenType::RESERVED_WORD, str, pos, currentPos()),
                                        std::optional<CompilationError>());
                            return std::make_pair(
                                    std::make_optional<Token>(TokenType::IDENTIFIER, str, pos, currentPos()),
                                    std::optional<CompilationError>());
                        }
                        break;
                    }

                        // 如果当前状态是加号
                    case PLUS_SIGN_STATE: {
                        // 请思考这里为什么要回退，在其他地方会不会需要
                        unreadLast(); // Yes, we unread last char even if it's an EOF.
                        return std::make_pair(std::make_optional<Token>(TokenType::PLUS_SIGN, '+', pos, currentPos()),
                                              std::optional<CompilationError>());
                    }
                        // 当前状态为减号的状态
                    case MINUS_SIGN_STATE: {
                        // 填空：回退，并返回减号token
                        unreadLast(); // Yes, we unread last char even if it's an EOF.
                        return std::make_pair(std::make_optional<Token>(TokenType::MINUS_SIGN, '-', pos, currentPos()),
                                              std::optional<CompilationError>());
                    }

                        // 请填空：
                        // 对于其他的合法状态，进行合适的操作
                        // 比如进行解析、返回token、返回编译错误

                    case DIVISION_SIGN_STATE: {
                        if (current_char == '/') {
                            std::string str = "//";
                            current_char = nextChar();
                            while (current_char.has_value() && (current_char != '\n' && current_char != '\r'))current_char = nextChar();
                            unreadLast();
                            return std::make_pair(
                                    std::make_optional<Token>(TokenType::SINGLE_LINE_COMMENT_SIGN, str, pos,
                                                              currentPos()),
                                    std::optional<CompilationError>());
                        }
                        if (current_char == '*') {
                            std::string str = "/*";
                            current_char = nextChar();
                            while(current_char.has_value()){
                                if(current_char == '*'){
                                    current_char = nextChar();
                                    if(current_char == '/') break;
                                }else
                                    current_char = nextChar();

                                if(!current_char.has_value())
                                    return std::make_pair(std::optional<Token>(),
                                                          std::make_optional<CompilationError>(pos, ErrorCode::ErrNeedRightMultipicationSign));
                            }
                            return std::make_pair(
                                    std::make_optional<Token>(TokenType::MULTI_LINE_COMMENT_LEFT, str, pos,
                                                              currentPos()),
                                    std::optional<CompilationError>());
                        }
                        unreadLast();
                        return std::make_pair(
                                std::make_optional<Token>(TokenType::DIVISION_SIGN, '/', pos, currentPos()),
                                std::optional<CompilationError>());
                    }
                    case MULTIPLICATION_SIGN_STATE: {
//                        if (current_char == '/') {
//                            std::string str = "*/";
//                            return std::make_pair(
//                                    std::make_optional<Token>(TokenType::MULTI_LINE_COMMENT_RIGHT, str, pos,
//                                                              currentPos()),
//                                    std::optional<CompilationError>());
//                        }
                        unreadLast();
                        return std::make_pair(
                                std::make_optional<Token>(TokenType::MULTIPLICATION_SIGN, '*', pos, currentPos()),
                                std::optional<CompilationError>());
                    }
                    case EQUAL_SIGN_STATE: {
                        if (current_char == '=') {
                            std::string str = "==";
                            return std::make_pair(
                                    std::make_optional<Token>(TokenType::DOUBLE_EQUAL_SIGN, str, pos, currentPos()),
                                    std::optional<CompilationError>());
                        }
                        unreadLast();
                        return std::make_pair(std::make_optional<Token>(TokenType::EQUAL_SIGN, '=', pos, currentPos()),
                                              std::optional<CompilationError>());
                    }
                    case SEMICOLON_STATE: {
                        unreadLast();
                        return std::make_pair(std::make_optional<Token>(TokenType::SEMICOLON, ';', pos, currentPos()),
                                              std::optional<CompilationError>());
                    }
                    case LEFTBRACKET_STATE: {
                        unreadLast();
                        return std::make_pair(
                                std::make_optional<Token>(TokenType::LEFT_BRACKET, '(', pos, currentPos()),
                                std::optional<CompilationError>());
                    }
                    case RIGHTBRACKET_STATE: {
                        unreadLast();
                        return std::make_pair(
                                std::make_optional<Token>(TokenType::RIGHT_BRACKET, ')', pos, currentPos()),
                                std::optional<CompilationError>());
                    }
                    case LESS_STATE: {
                        if (current_char == '=') {
                            std::string str = "<=";
                            return std::make_pair(
                                    std::make_optional<Token>(TokenType::LESS_EQUAL_SIGN, str, pos, currentPos()),
                                    std::optional<CompilationError>());
                        }
                        unreadLast();
                        return std::make_pair(std::make_optional<Token>(TokenType::LESS_SIGN, '<', pos, currentPos()),
                                              std::optional<CompilationError>());
                    }
                    case GREATER_STATE: {
                        if (current_char == '=') {
                            std::string str = ">=";
                            return std::make_pair(
                                    std::make_optional<Token>(TokenType::GREATER_EQUAL_SIGN, str, pos, currentPos()),
                                    std::optional<CompilationError>());
                        }
                        unreadLast();
                        return std::make_pair(
                                std::make_optional<Token>(TokenType::GREATER_SIGN, '>', pos, currentPos()),
                                std::optional<CompilationError>());
                    }
                    case EXCLAMATION_STATE: {
                        if (current_char == '=') {
                            std::string str = "!=";
                            return std::make_pair(
                                    std::make_optional<Token>(TokenType::UNEQUAL_SIGN, str, pos, currentPos()),
                                    std::optional<CompilationError>());
                        }
                        return std::make_pair(std::optional<Token>(),
                                              std::make_optional<CompilationError>(pos, ErrorCode::ErrInvalidInput));
                    }
                    case DOUBLE_QUOTES_STATE: {
                        std::string string;
                        char ch1;
                        auto ch = current_char.value();
                        ss>>ch1;
                        while(current_char.has_value() && current_char != '\"'){
                            ch = current_char.value();
                            if(ch == '\'' || ch == '\r'|| ch == '\n')
                                return std::make_pair(std::optional<Token>(),
                                                      std::make_optional<CompilationError>(pos, ErrorCode::ErrInvalidInput));
                            if(ch == '\\'){
                                unreadLast();
                                std::string str = isNextEscapeSeqRetStr();
                                if(str != "-1")
                                {
                                    ss<<str;
                                }
                                else return std::make_pair(std::optional<Token>(),
                                                           std::make_optional<CompilationError>(pos, ErrorCode::ErrInvalidInput));
                            }
                            else ss<<ch;
                            current_char = nextChar();
                        }
                        if(!current_char.has_value() )
                            return std::make_pair(std::optional<Token>(),
                                                  std::make_optional<CompilationError>(pos, ErrorCode::ErrNeedDoubleQuotes));
                        getline(ss,string);
                        return std::make_pair(
                                std::make_optional<Token>(TokenType::STRING_SIGN, string, pos, currentPos()),
                                std::optional<CompilationError>());
                    }
                    case SINGLE_QUOTE_STATE: {
                        auto ch = current_char.value();
                        if(ch == '\\'){
                            unreadLast();
                            int num = isNextEscapeSeq();
                            if(num >= 0)
                            {
                                current_char = nextChar();
                                if(!current_char.has_value()|| current_char.value() != '\'')
                                    return std::make_pair(std::optional<Token>(),
                                                          std::make_optional<CompilationError>(pos, ErrorCode::ErrNeedSingalQuote));
                                return std::make_pair(
                                        std::make_optional<Token>(TokenType::CHAR_SIGN, (char)num, pos, currentPos()),
                                        std::optional<CompilationError>());
                            }
                            else return std::make_pair(std::optional<Token>(),
                                                       std::make_optional<CompilationError>(pos, ErrorCode::ErrInvalidInput));
                        }
                        if(ch == '\"' || ch == '\r'|| ch == '\n')
                            return std::make_pair(std::optional<Token>(),
                                                  std::make_optional<CompilationError>(pos, ErrorCode::ErrInvalidInput));
                        char c;
                        std::string string;
                        ss>>c;
                        ss<<ch;
                        getline(ss,string);
                        current_char = nextChar();
                        if(!current_char.has_value()|| current_char.value() != '\'')
                            return std::make_pair(std::optional<Token>(),
                                                  std::make_optional<CompilationError>(pos, ErrorCode::ErrNeedSingalQuote));
                        return std::make_pair(
                                std::make_optional<Token>(TokenType::CHAR_SIGN, string, pos, currentPos()),
                                std::optional<CompilationError>());
                    }
                    case BIG_LEFTBRACKET_STATE: {
                        unreadLast();
                        return std::make_pair(
                                std::make_optional<Token>(TokenType::BIG_LEFT_BRACKET, '{', pos, currentPos()),
                                std::optional<CompilationError>());
                    }
                    case BIG_RIGHTBRACKET_STATE: {
                        unreadLast();
                        return std::make_pair(
                                std::make_optional<Token>(TokenType::BIG_RIGHT_BRACKET, '}', pos, currentPos()),
                                std::optional<CompilationError>());
                    }
                    case COMMA_STATE: {
                        unreadLast();
                        return std::make_pair(std::make_optional<Token>(TokenType::COMMA_SIGN, ',', pos, currentPos()),
                                              std::optional<CompilationError>());
                    }
                        // 预料之外的状态，如果执行到了这里，说明程序异常
                    default:
                        DieAndPrint("unhandled state.");
                        break;
                }
            }
            // 预料之外的状态，如果执行到了这里，说明程序异常
            return std::make_pair(std::optional<Token>(), std::optional<CompilationError>());
        }

        std::optional<CompilationError> Tokenizer::checkToken(const Token &t) {
            switch (t.GetType()) {
                case IDENTIFIER: {
                    auto val = t.GetValueString();
                    if (LNC0::isdigit(val[0]))
                        return std::make_optional<CompilationError>(t.GetStartPos().first, t.GetStartPos().second,
                                                                    ErrorCode::ErrInvalidIdentifier);
                    break;
                }
                default:
                    break;
            }
            return {};
        }

        void Tokenizer::readAll() {
            if (_initialized)
                return;
            for (std::string tp; std::getline(_rdr, tp);)
                _lines_buffer.emplace_back(std::move(tp + "\n"));
            _initialized = true;
            _ptr = std::make_pair<int64_t, int64_t>(0, 0);
            return;
        }

        // Note: We allow this function to return a postion which is out of bound according to the design like std::vector::end().
        std::pair<uint64_t, uint64_t> Tokenizer::nextPos() {
            if (_ptr.first >= _lines_buffer.size())
                DieAndPrint("advance after EOF");
            if (_ptr.second == _lines_buffer[_ptr.first].size() - 1)
                return std::make_pair(_ptr.first + 1, 0);
            else
                return std::make_pair(_ptr.first, _ptr.second + 1);
        }

        std::pair<uint64_t, uint64_t> Tokenizer::currentPos() {
            return _ptr;
        }

        std::pair<uint64_t, uint64_t> Tokenizer::previousPos() {
            if (_ptr.first == 0 && _ptr.second == 0)
                DieAndPrint("previous position from beginning");
            if (_ptr.second == 0)
                return std::make_pair(_ptr.first - 1, _lines_buffer[_ptr.first - 1].size() - 1);
            else
                return std::make_pair(_ptr.first, _ptr.second - 1);
        }

        std::optional<char> Tokenizer::nextChar() {
            if (isEOF())
                return {}; // EOF
            auto result = _lines_buffer[_ptr.first][_ptr.second];
            _ptr = nextPos();
            return result;
        }
        bool Tokenizer::isEOF() {
            return _ptr.first >= _lines_buffer.size();
        }

        // Note: Is it evil to unread a buffer?
        void Tokenizer::unreadLast() {
            _ptr = previousPos();
        }

    int32_t Tokenizer::isNextEscapeSeq() {
        auto current_char = nextChar();
        if(current_char.has_value() && current_char.value() =='\\'){
            current_char = nextChar();
            if(current_char.has_value() && current_char.value() =='\\'){
                return 92;
            }else if(current_char.has_value() && current_char.value() =='\''){
                return 39;
            }else if(current_char.has_value() && current_char.value() =='\"'){
                return 34;
            }else if(current_char.has_value() && current_char.value() =='n'){
                return 10;
            }else if(current_char.has_value() && current_char.value() =='r'){
                return 13;
            }else if(current_char.has_value() && current_char.value() =='t'){
                return 9;
            }else if(current_char.has_value() && current_char.value() =='x'){
                current_char = nextChar();
                if(!current_char.has_value()) {
                    return -1;
                }
                if((current_char >= '0' && current_char <= '9')
                || (current_char >= 'a' && current_char <= 'f')
                || (current_char >= 'A' && current_char <= 'F'))
                {
                    std::string string;
                    string[0] = current_char.value();
                    current_char = nextChar();
                    if(!current_char.has_value()) return -1;
                    if((current_char >= '0' && current_char <= '9')
                       || (current_char >= 'a' && current_char <= 'f')
                       || (current_char >= 'A' && current_char <= 'F'))
                    {
                        string[1] = current_char.value();
                        unsigned int num;
                        sscanf(string.c_str(),"%x",&num);
                        return (int32_t) num;
                    } else return -1;
                } else return -1;
            }
            else{
                unreadLast();
                unreadLast();
                return -1;
            }
        }else{
            unreadLast();
            return -1;
        }

    }
    std::string Tokenizer::isNextEscapeSeqRetStr() {
        auto current_char = nextChar();
        if(current_char.has_value() && current_char.value() =='\\'){
            current_char = nextChar();
            if(current_char.has_value() && current_char.value() =='\\'){
                return "\\\\";
            }else if(current_char.has_value() && current_char.value() =='\''){
                return "\\\'";
            }else if(current_char.has_value() && current_char.value() =='\"'){
                return "\\\"";
            }else if(current_char.has_value() && current_char.value() =='n'){
                return "\\n";
            }else if(current_char.has_value() && current_char.value() =='r'){
                return "\\r";
            }else if(current_char.has_value() && current_char.value() =='t'){
                return "\\t";
            }else if(current_char.has_value() && current_char.value() =='x'){
                current_char = nextChar();
                if(!current_char.has_value()) {
                    return "-1";
                }
                if((current_char >= '0' && current_char <= '9')
                   || (current_char >= 'a' && current_char <= 'f')
                   || (current_char >= 'A' && current_char <= 'F'))
                {
                    char string[5];
                    string[0] = '\\';
                    string[1] = 'x';
                    string[2] = current_char.value();
                    current_char = nextChar();
                    if(!current_char.has_value()) return "-1";
                    if((current_char >= '0' && current_char <= '9')
                       || (current_char >= 'a' && current_char <= 'f')
                       || (current_char >= 'A' && current_char <= 'F'))
                    {
                        string[3] = current_char.value();
                        return string;
                    } else return "-1";
                } else return "-1";
            }
            else{
                unreadLast();
                unreadLast();
                return "-1";
            }
        }else{
            unreadLast();
            return "-1";
        }

    }

        bool Tokenizer::isReservedWord(const std::string& basicString) {
            return basicString == "const" ||
                   basicString == "void" ||
                   basicString == "int" ||
                   basicString == "char" ||
                   basicString == "double" ||
                   basicString == "struct" ||
                   basicString == "if" ||
                   basicString == "else" ||
                   basicString == "switch" ||
                   basicString == "case" ||
                   basicString == "default" ||
                   basicString == "while" ||
                   basicString == "for" ||
                   basicString == "do" ||
                   basicString == "return" ||
                   basicString == "break" ||
                   basicString == "continue" ||
                   basicString == "print" ||
                   basicString == "scan";

        }
    }