选做：
	注释 1

	char+类型转换+字符字面量+字符串字面量 9

	多级作用域 3

	break+continue+(do/for) 3/5

	break+switch 5

	double+浮点数字面量 10
​	



/************************************************************************以下是词法***************************************************************************/

```


<数字> ::=  '0'|<非零_数字>_

_<非零_数字> ::=  '1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'

<十六进制_数字> ::= <数字>|'a'|'b'|'c'|'d'|'e'|'f'|'A'|'B'|'C'|'D'|'E'|'F'

<整数_串> ::=  <十进制_串>|<十六进制_串>

<十进制_串> ::=  '0'|<非零_数字>{<数字>}

<十六进制_串> ::=  ('0x'|'0X')<十六进制_数字>{<十六进制_数字>}


<字母> ::=    'a'|'b'|'c'|'d'|'e'|'f'|'g'|'h'|'i'|'j'|'k'|'l'|'m'|'n'|'o'|'p'|'q'|'r'|'s'|'t'|'u'|'v'|'w'|'x'|'y'|'z'|'A'|'B'|'C'|'D'|'E'|'F'|'G'|'H'|'I'|'J'|'K'|'L'|'M'|'N'|'O'|'P'|'Q'|'R'|'S'|'T'|'U'|'V'|'W'|'X'|'Y'|'Z'

<标识符> ::=  <字母>{<字母>|<数字>}

<保留字> ::=   'const' |'void'   |'int'    |'char'   |'double' |'struct' |'if'     |'else'    |'switch' |'case'   |'default' |'while'  |'for'    |'do' |'return' |'break'  |'continue'  |'print'  |'scan'


<char_串> ::=  "'" (<c_char>|<escape_序列>) "'" 

<string_串> ::=  '"' {<s_char>|<escape_序列>} '"'

<escape_序列> ::=     '\\' | "\'" | '\"' | '\n' | '\r' | '\t' | '\x'<十六进制_数字><十六进制_数字>

<单目> ::=  '+'|'_'_

_<数字_序列> ::= <数字>{<数字>}

<浮点数_串> ::=   [<数字_序列>]'.'<数字_序列>[<指数>] |<数字_序列>'.'[<指数>] |<数字_序列><指数>

<指数> ::=  ('e'|'E')[<单目>]<数字_序列>

<单目_操作符>     ::= '+' | '_'

<加法_操作符>       ::= '+' | '_'

<乘法_操作符> 		::= '*' | '/'

<关系_操作符>     	::= '<' | '<=' | '>' | '>=' | '!=' | '=='

<赋值_操作符>     	::= '='   

<单行注释> ::= '//'{<any_char>}<LF>

<多行注释> ::=  '/*'{<any_char>}'*/'    

<类型_说明符号>         ::= <简单_类型_说明符号>

<简单_类型_说明符号>  ::= 'void'|'int'|'char'|'double'

<const_修饰符>        ::= 'const'

/****************************************************************************以上是词法***************************************************************************/

/****************************************************************************以下是语法***************************************************************************/   

<C0_program> ::=  {<变量_声明>}{<函数_定义>}

<C0_program> ::=  {<variable_declaration>}{<function_definition>}



<变量_声明> ::=  [<const_修饰符>]<类型_说明符号><声明_语句_列表>';'

<variable_declaration> ::=  [<const_qualifier>]<type_specifier><init_declarator_list>';'

<声明_语句_列表> ::=  <声明_语句>{','<声明_语句>}

<init_declarator_list> ::=  <init_declarator>{','<init_declarator>}	

<声明_语句> ::=  <标识符>[<初始化语句>]

<init_declarator> ::=  <identifier>[<initializer>]	

<初始化语句> ::=  '='<表达式>    

<initializer> ::=  '='<expression>



  

<函数_定义> ::=  <类型_说明符号><标识符><参数_clause><函数体_语句>
<function_definition> ::=  <type_specifier><identifier><parameter_clause><compound_statement>

<参数_clause> ::=  '(' [<参数_声明_列表>] ')'
<parameter_clause> ::=  '(' [<parameter_declaration_list>] ')'	

<参数_声明_列表> ::=  <参数_声明>{','<参数_声明>}
<parameter_declaration_list> ::=  <parameter_declaration>{','<parameter_declaration>}	

<参数_声明> ::=  [<const_修饰符>]<类型_说明符号><标识符>
<parameter_declaration> ::=  [<const_qualifier>]<type_specifier><identifier>



<函数体_语句> ::=  '{' {<变量_声明>} <语句_序列> '}'
<compound_statement> ::=  '{' {<variable_declaration>} <statement_seq> '}'

<语句_序列> ::= {<语句>}
<statement_seq> ::= {<statement>}

<语句> ::=   <函数体_语句> |<条件_语句> |<loop_语句> |<jump_语句> |<print_语句> |<scan_语句> |<赋值_表达式>';' |<函数_call>';' |';'   

<statement> ::=   <compound_statement> |<condition_statement> |<loop_statement> |<jump_statement> |<print_statement> |<scan_statement> |<assignment_expression>';' |<function_call>';' |';'      





<条件> ::=   <表达式>[<关系_操作符><表达式>] 

<condition> ::=   <expression>[<relational_operator><expression>]    

<条件_语句> ::=   'if' '(' <条件> ')' <语句> ['else' <语句>] |'switch' '(' <表达式> ')' '{' {<labeled_语句>} '}'

<condition_statement> ::=   'if' '(' <condition> ')' <statement> ['else' <statement>] |'switch' '(' <expression> ')' '{' {<labeled_statement>} '}'

<labeled_语句> ::=   'case' (<整数_串>|<char_串>) ':' <语句> |'default' ':' <语句>

<labeled_statement> ::=   'case' (<integer_literal>|<char_literal>) ':' <statement> |'default' ':' <statement>

<loop_语句> ::=  'while' '(' <条件> ')' <语句>
   |'do' <语句> 'while' '(' <条件> ')' ';'
   |'for' '('<for_声明_语句> [<条件>]';' [<for_update_表达式>]')' <语句>

<loop_statement> ::=  'while' '(' <condition> ')' <statement>
   |'do' <statement> 'while' '(' <condition> ')' ';'
   |'for' '('<for_init_statement> [<condition>]';' [<for_update_expression>]')' <statement>

<for_声明_语句> ::=  [<赋值_表达式>{','<赋值_表达式>}]';'

<for_init_statement> ::=  [<assignment_expression>{','<assignment_expression>}]';'

<for_update_表达式> ::= (<赋值_表达式>|<函数_call>){','(<赋值_表达式>|<函数_call>)}

<for_update_expression> ::= (<assignment_expression>|<function_call>){','(<assignment_expression>|<function_call>)}

<jump_语句> ::=   'break' ';' |'continue' ';' |<return_语句>

<return_语句> ::= 'return' [<表达式>] ';'

<jump_statement> ::=   'break' ';' |'continue' ';' |<return_statement>

<return_statement> ::= 'return' [<expression>] ';'  





<scan_语句> ::=  'scan' '(' <标识符> ')' ';'

<scan_statement> ::=  'scan' '(' <identifier> ')' ';'

<print_语句> ::=  'print' '(' [<printable_列表>] ')' ';'

<print_statement> ::=  'print' '(' [<printable_list>] ')' ';'

<printable_列表>  ::=  <printable> {',' <printable>}

<printable_list>  ::=  <printable> {',' <printable>}

<printable> ::=  <表达式> | <string_串>

<printable> ::=  <expression> | <string_literal>




<赋值_表达式> ::=  <标识符><赋值_操作符><表达式>

<assignment_expression> ::=  <identifier><assignment_operator><expression>     

<表达式> ::=  <加法_表达式>

<expression> ::=  <additive_expression>

<加法_表达式> ::=   <乘法_表达式>{<加法_操作符><乘法_表达式>}

<additive_expression> ::=   <multiplicative_expression>{<additive_operator><multiplicative_expression>}

<乘法_表达式> ::=   <cast_表达式>{<乘法_操作符><cast_表达式>}

<multiplicative_expression> ::=   <cast_expression>{<multiplicative_operator><cast_expression>}

<cast_表达式> ::= {'('<类型_说明符号>')'}<单目_表达式>

<cast_expression> ::= {'('<type_specifier>')'}<unary_expression>

<单目_表达式> ::= [<单目_操作符>]<primary_表达式>

<unary_expression> ::= [<unary_operator>]<primary_expression>

<primary_表达式> ::=    '('<表达式>')'  |<标识符> |<整数_串> |<char_串> |<浮点数_串> |<函数_call>

<primary_expression> ::=    '('<expression>')'  |<identifier> |<integer_literal> |<char_literal> |<floating_literal> |<function_call>

<函数_call> ::=  <标识符> '(' [<表达式_列表>] ')'

<function_call> ::=  <identifier> '(' [<expression_list>] ')'

<表达式_列表> ::=  <表达式>{','<表达式>}

<expression_list> ::=  <expression>{','<expression>}

```