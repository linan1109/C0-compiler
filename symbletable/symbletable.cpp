#include "symbletable.h"


namespace miniplc0 {

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
        while(father != nullptr)
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
        while(father != nullptr)
        {
            for(long long unsigned int i = 0; i < symbleTable.List.size();i++)
                if(s == symbleTable.List[i].getName())
                {
                    pii.first = symbleTable.List[i].getAllIndex();
                    return pii;
                }
            if(symbleTable.father == nullptr) break;
            symbleTable = *symbleTable.father;
            pii.second++;
        }
        return pii;
    }
    int32_t SymbleTable:: getKindByName(const std::string &s){
        SymbleTable symbleTable = *this;
        while(father != nullptr)
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
        while(father != nullptr)
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

    int32_t SymbleTable:: getSizeByName(const std::string &s){
        SymbleTable symbleTable = *this;
        while(father != nullptr)
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
    bool SymbleTable::operator==(const SymbleTable &rhs) const {
        return List == rhs.List &&
               List.size() == rhs.List.size() &&
               father == rhs.father;
    }

    bool SymbleTable::operator!=(const SymbleTable &rhs) const {
        return !(rhs == *this);
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

    const std::vector<std::pair<std::string, int32_t>> &function::getParams() const {
        return _params;
    }

    void function::setParams(const std::vector<std::pair<std::string, int32_t>> &params) {
        _params = params;
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

    function::function(const std::string &name, int32_t type) : _name(name), _type(type) {}
}