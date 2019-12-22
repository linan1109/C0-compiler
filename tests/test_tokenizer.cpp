#include "catch2/catch.hpp"
#include "tokenizer/tokenizer.h"
#include "fmt/core.h"
#include <sstream>
#include <vector>

// 下面是示例如何书写测试用例
TEST_CASE("Test hello world.") {
//
//	std::string input ="- + * /\n"
//                       "+-*/\n"
//                       "++ --\n"
//                       "= += -= *= /= ==\n"
//                       "//\n"
//                       "**\n"
//                       "/**/\n"
//                       "/*/";
//		"begin\n"
//		"	var a=1;\n"
//		"	const b=2\n"
//		"	print(a+b);\n"
//		"end\n";
//	std::stringstream ss;
//	ss.str(input);
//	miniplc0::Tokenizer tkz(ss);
//	std::vector<miniplc0::Token> output = {};
//	auto result = tkz.AllTokens();
//	//shan
//	std::cout <<"-----------------starting------------------"<<std::endl;
//	std::cout << "count\t" << result.first.size()<<std::endl;
//	for(miniplc0::Token i : result.first){
//	    std::cout << i.GetType() <<"\t" << i.GetValueString() <<std::endl;
//
////        std::cout << i.GetType() <<std::endl;
//	}
//	//shanwan
//	if (result.second.has_value()) {
//		FAIL();
//	}
//	REQUIRE( (result.first == output) );

}