#include "symbletable.h"


namespace LNC0 {

    bool SymbleTable::addSymble(const std::string &s, int32_t kind, int32_t type, int32_t value, int32_t size){
        if(isDeclared(s))return false;
        List.emplace_back(s,kind,type,value,size);
        return true;
    }

    bool SymbleTable::changeSizeOfFun(const std::string &s, int32_t size){
        for(long long unsigned int i = 0; i < List.size();i++){
            if(s == List[i].getName()){
                List[i].setSize(size);
                return true;
            }
        }
        return false;
    }

    bool SymbleTable::isDeclared(const std::string &s) {
        for (long long unsigned int i = 0; i < List.size(); i++) {
            if (s == List[i].getName())
                return true;
        }
        return false;
    }

    one_symbol * SymbleTable:: getByName(const std::string &s){
        SymbleTable symbleTable = *this;
        while(true)
        {
            for(long long unsigned int i = 0; i < symbleTable.List.size();i++)
                if(s == symbleTable.List[i].getName())
                {
                    return &symbleTable.List[i];
                }
            if(symbleTable.father == nullptr) break;
            symbleTable = *symbleTable.father;
        }
        one_symbol * ss = new one_symbol("", -1, -1, -1, -1);
        return ss;
    }

    std::pair<int32_t ,int32_t >SymbleTable:: index_in_all(const std::string &s){
        //first->index second->father count
        SymbleTable symbleTable = *this;
        std::pair<int32_t ,int32_t > pii;
        pii.first = -1;
        pii.second = 0;
        while(true)
        {
            for(long long unsigned int i = 0; i < symbleTable.List.size();i++)
                if(s == symbleTable.List[i].getName())
                {
                    pii.first = i + this->start_index;
                    return pii;
                }
            if(symbleTable.father == nullptr) break;
            symbleTable = *symbleTable.father;
            pii.second++;
        }
        return pii;
    }

    void SymbleTable:: changeStartIndex(Operation o){
        switch (o) {
            case LNC0::iadd:
            case LNC0::istore:
            case LNC0::isub:
            case LNC0::imul:
            case LNC0::idiv:
            case LNC0::iprint:
            case LNC0::cprint:
            case LNC0::sprint:
                now_index--;
                break;
            case LNC0::ineg:
            case LNC0::i2c:
            case LNC0::iload:
            case LNC0::nop:
            case LNC0::ret:
            case LNC0::iret:
            case LNC0::jmp:
            case LNC0::call:
            case LNC0::je:
            case LNC0::jne:
            case LNC0::jle:
            case LNC0::jl:
            case LNC0::jg:
            case LNC0::jge:
            case LNC0::_F:
            case LNC0::_start:
                break;
            case LNC0::loadc:
            case LNC0::iscan:
            case LNC0::cscan:
            case LNC0::ipush:
            case LNC0::bipush:
            case LNC0::loada:
                now_index++;
                break;


            default: break;



        }
    }

    int32_t SymbleTable:: getLevel(){
        SymbleTable symbleTable = *this;
        int i = 0;
        while(true)
        {
            if(symbleTable.father == nullptr) return i;
            i++;
            symbleTable = *symbleTable.father;
        }
        return -1;
    }
    int32_t SymbleTable:: getKindByName(const std::string &s){
        SymbleTable symbleTable = *this;
        while(true)
        {
            for(long long unsigned int i = 0; i < symbleTable.List.size();i++)
                if(s == symbleTable.List[i].getName())
                {
                    return symbleTable.List[i].getKind();
                }
            if(symbleTable.father == nullptr) break;
            symbleTable = *symbleTable.father;
        }
        return -1;
    }
    int32_t SymbleTable:: getTypeByName(const std::string &s){
        SymbleTable symbleTable = *this;
        while(true)
        {
            for(long long unsigned int i = 0; i < symbleTable.List.size();i++)
                if(s == symbleTable.List[i].getName())
                {
                    return symbleTable.List[i].getType();
                }
            if(symbleTable.father == nullptr) break;
            symbleTable = *symbleTable.father;
        }
        return -1;
    }
    int32_t SymbleTable:: setValueByName(const std::string &s){
        SymbleTable *symbleTable = this;
        while(true)
        {
            for(long long unsigned int i = 0; i < symbleTable->List.size();i++)
                if(s == symbleTable->List[i].getName())
                {
                    symbleTable->List[i].setValue(1);
                    return i;
                }
            if(symbleTable->father == nullptr) break;
            symbleTable = symbleTable->father;
        }
        return -1;
    }
    int32_t SymbleTable:: getValueByName(const std::string &s){
        SymbleTable symbleTable = *this;
        while(true)
        {
            for(long long unsigned int i = 0; i < symbleTable.List.size();i++)
                if(s == symbleTable.List[i].getName())
                {
                    return symbleTable.List[i].getValue();
                }
            if(symbleTable.father == nullptr) break;
            symbleTable = *symbleTable.father;
        }
        return -1;
    }
    int32_t SymbleTable:: getSizeByName(const std::string &s){
        SymbleTable symbleTable = *this;
        while(true)
        {
            for(long long unsigned int i = 0; i < symbleTable.List.size();i++)
                if(s == symbleTable.List[i].getName())
                {
                    return symbleTable.List[i].getSize();
                }
            if(symbleTable.father == nullptr) break;
            symbleTable = *symbleTable.father;
        }
        return -1;
    }


    const std::string &SymbleTable::getName() const {
        return name;
    }

    void SymbleTable::setName(const std::string &name) {
        SymbleTable::name = name;
    }

    bool SymbleTable::operator==(const SymbleTable &rhs) const {
        return name == rhs.name &&
               List == rhs.List &&
               father == rhs.father;
    }

    bool SymbleTable::operator!=(const SymbleTable &rhs) const {
        return !(rhs == *this);
    }

    SymbleTable *SymbleTable::getFather() const {
        return father;
    }

    void SymbleTable::setFather(SymbleTable *father) {
        SymbleTable::father = father;
    }


    one_symbol::one_symbol(const std::string &name, int32_t kind, int32_t type, int32_t value, int32_t size
                           ) : _name(name), _kind(kind), _type(type), _value(value), _size(size) {}

    const std::string &one_symbol::getName() const {
        return _name;
    }

    void one_symbol::setName(const std::string &name) {
        _name = name;
    }

    int32_t one_symbol::getKind() const {
        return _kind;
    }

    void one_symbol::setKind(int32_t kind) {
        _kind = kind;
    }

    int32_t one_symbol::getType() const {
        return _type;
    }

    void one_symbol::setType(int32_t type) {
        _type = type;
    }

    int32_t one_symbol::getValue() const {
        return _value;
    }

    void one_symbol::setValue(int32_t value) {
        _value = value;
    }

    int32_t one_symbol::getSize() const {
        return _size;
    }

    void one_symbol::setSize(int32_t size) {
        _size = size;
    }

    int32_t one_symbol::getAllIndex() const {
        return all_index;
    }

    void one_symbol::setAllIndex(int32_t allIndex) {
        all_index = allIndex;
    }

    bool one_symbol::operator==(const one_symbol &rhs) const {
        return _name == rhs._name &&
               _kind == rhs._kind &&
               _type == rhs._type &&
               _value == rhs._value &&
               _size == rhs._size &&
               all_index == rhs.all_index;
    }

    bool one_symbol::operator!=(const one_symbol &rhs) const {
        return !(rhs == *this);
    }


    const std::string &function::getName() const {
        return _name;
    }

    void function::setName(const std::string &name) {
        _name = name;
    }

    int32_t function::getType() const {
        return _type;
    }

    void function::setType(int32_t type) {
        _type = type;
    }

    bool function::operator==(const function &rhs) const {
        return _name == rhs._name &&
               _params == rhs._params &&
               _type == rhs._type;
    }

    bool function::operator!=(const function &rhs) const {
        return !(rhs == *this);
    }

    bool function::addParam(const std::string &name, int32_t kind,int32_t type){
        for(unsigned long long int i = 0; i < _params.size(); i++){
            if(name == _params[i].first)
                return false;
        }
        std::pair<std::string, std::pair<int32_t , int32_t > > pii;
        pii.first = name;
        pii.second.first = kind;
        pii.second.second = type;
        _params.push_back(pii);
        return true;
    }
    void function::addInstruction(Operation operation, int32_t x, int32_t y){
        _instructions.emplace_back(_count++,operation,x,y);
    }

    const std::vector<std::pair<std::string, std::pair<std::int32_t, int32_t>>> &function::getParams() const {
        return _params;
    }

    void function::setParams(const std::vector<std::pair<std::string, std::pair<std::int32_t, int32_t>>> &params) {
        _params = params;
    }

    const std::vector<Instruction> &function::getInstructions() const {
        return _instructions;
    }

    void function::setInstructions(const std::vector<Instruction> &instructions) {
        _instructions = instructions;
    }

    function::function(const std::string &name, int32_t type, int32_t nameIndex, int32_t level) : _name(name),
                                                                                                  _type(type),
                                                                                                  _level(level),
                                                                                                  _name_index(
                                                                                                          nameIndex) {
        _count = 0;
    }

    int32_t function::getLevel() const {
        return _level;
    }

    void function::setLevel(int32_t level) {
        _level = level;
    }

    int32_t function::getNameIndex() const {
        return _name_index;
    }

    void function::setNameIndex(int32_t nameIndex) {
        _name_index = nameIndex;
    }

    bool function::setInstructionX(int32_t Index, int32_t x) {
        _instructions[Index].setX(x);
        return true;
    }


    int32_t function::getCount() const {
        return _count;
    }

    void function::setCount(int32_t count) {
        _count = count;
    }

    int32_t function::addCount() {
        _count++;
        return _count-1;
    }

}