set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.15)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_library(LIB_SRC

        c0_vm.h

        print.hpp
        tuple_visit.hpp
        util.hpp

        type.h
        opcode.h
        instruction.h
        constant.h
        function.h
        exception.h

        file.h
        file.cpp

        vm.h
        vm.cpp


        )

