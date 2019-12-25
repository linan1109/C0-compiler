#pragma once

#include <cstdint>
#include <utility>

namespace LNC0 {

	enum Operation {
	    loada,
        ipush,
        istore,
        iadd,
        isub,
        imul,
        idiv,
        ineg,
        i2c,
        iload,
        jmp,
        bipush,
        iscan,
        cscan,
        iprint,
        cprint,
        sprint,
        loadc,
        ret,
        iret,
        call,
        _F,
        _start,
        je,
        jne,
        jl,
        jge,
        jg,
        jle,
        nop,
    };

	class Instruction final {
	private:
		using int32_t = std::int32_t;
	public:
		friend void swap(Instruction& lhs, Instruction& rhs);
	public:
		Instruction(int32_t count, Operation opr, int32_t x, int32_t y) : _opr(opr), _x(x), _y(y),_count(count) {}

		Instruction() : Instruction(0,Operation::iload, 0,0){}
		Instruction(const Instruction& i) {_count = i._count; _opr = i._opr; _x = i._x; _y = i._y;}
		Instruction(Instruction&& i) :Instruction() { swap(*this, i); }
		Instruction& operator=(Instruction i) { swap(*this, i); return *this; }
		bool operator==(const Instruction& i) const { return _opr == i._opr && _x == i._x && _y == i._y &&_count == i._count; }

		Operation GetOperation() const { return _opr; }
		int32_t GetX() const { return _x; }
        int32_t GetY() const { return _y; }

        int32_t GetCount() const { return _count; }

        void setX(int32_t x) {
            _x = x;
        }
	private:
		Operation _opr;
		int32_t _x;
        int32_t _y;
        int32_t _count;


    };

	inline void swap(Instruction& lhs, Instruction& rhs) {
		using std::swap;
		swap(lhs._opr, rhs._opr);
		swap(lhs._x, rhs._x);
		swap(lhs._y, rhs._y);
		swap(lhs._count,rhs._count);
	}


}