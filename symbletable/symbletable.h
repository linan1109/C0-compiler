#pragma once
#include <vector>
#include <optional>
#include <utility>
#include <cstdint>
#include <algorithm>
#include <instruction/instruction.h>

namespace miniplc0 {
    class one_symbol final{
        std::string _name;            //标识符名称
        int32_t _kind;                 /*种类
                                0：常量
                                1：变量
                                2：函数

                              */
        int32_t _type;                 /*类型
                                0：void(仅对于无返回值函数)
                                1：int32_t
                                2: char
                              */
        int32_t _value;       /*
                                未初始化：0
                                已初始化：1
                              */
        int32_t _size;                 /*函数：参数个数
                                数组：元素个数
                                其他：-1
                              */
        int32_t all_index;
    public:
        bool operator==(const one_symbol &rhs) const;

        bool operator!=(const one_symbol &rhs) const;

        one_symbol(const std::string &name, int32_t kind, int32_t type, int32_t value, int32_t size);

        const std::string &getName() const;

        void setName(const std::string &name);

        int32_t getKind() const;

        void setKind(int32_t kind);

        int32_t getType() const;

        void setType(int32_t type);

        int32_t getValue() const;

        void setValue(int32_t value);

        int32_t getSize() const;

        void setSize(int32_t size);

        int32_t getAllIndex() const;

        void setAllIndex(int32_t allIndex);
    };


    class SymbleTable final{
    private:
        std::string name;
        using int32_t = std::int32_t;
        std::vector<one_symbol> List; //符号表
        SymbleTable *father;

    public:
        SymbleTable *getFather() const;

        void setFather(SymbleTable *father);

    public:
        const std::string &getName() const;

        void setName(const std::string &name);



        SymbleTable(){
            name = "";
            father = nullptr;
        }

        SymbleTable(SymbleTable *_father,std::string _name){
            father = _father;
            name = _name;
        }

        bool operator==(const SymbleTable &rhs) const;

        bool operator!=(const SymbleTable &rhs) const;

        bool isDeclared(const std::__cxx11::basic_string<char> &s);

        int32_t getKindByName(const std::string &s);

        bool addSymble(const std::string &s, int32_t kind, int32_t type, int32_t value, int32_t size);

        int32_t getTypeByName(const std::string &s);

        int32_t getValueByName(const std::string &s);

        std::pair<int32_t, int32_t> index_in_all(const std::string &s);

        bool changeSizeOfFun(const std::string &s, int32_t size);

        one_symbol * getByName(const std::string &s);

        int32_t getSizeByName(const std::string &s);

        int32_t getLevel();

        int32_t setValueByName(const std::string &s);
    };

    class function{
    private:
        std::string _name;
        std::vector< std::pair<std::string, std::pair<std::int32_t , int32_t > > >_params;
        int32_t _type;
        std::vector<Instruction> _instructions;
        int32_t _level;
        int32_t _name_index;
        int32_t _count;

        //name
        //kind                 /*种类
        //                                0：常量
        //                                1：变量
        //type
        //                                0：void(仅对于无返回值函数)
        //                                1：int32_t
        //                                2: char
    public:
        int32_t getCount() const;

        void setCount(int32_t count);

        function(const std::string &name, int32_t type, int32_t nameIndex, int32_t level);

        bool operator==(const function &rhs) const;

        bool operator!=(const function &rhs) const;

        int32_t getLevel() const;

        void setLevel(int32_t level);

        int32_t getNameIndex() const;

        void setNameIndex(int32_t nameIndex);

        const std::string &getName() const;

        void setName(const std::string &name);

        int32_t getType() const;

        const std::vector<Instruction> &getInstructions() const;

        void setInstructions(const std::vector<Instruction> &instructions);

        void setType(int32_t type);

        const std::vector<std::pair<std::string, std::pair<std::int32_t, int32_t>>> &getParams() const;

        void setParams(const std::vector<std::pair<std::string, std::pair<std::int32_t, int32_t>>> &params);

        bool addParam(const std::string &name, int32_t kind, int32_t type);

        bool setInstructionX(int32_t Index, int32_t x);
        int32_t addCount();

        void addInstruction(Operation operation, int32_t x, int32_t y);
    };
}

