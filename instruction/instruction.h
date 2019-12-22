#pragma once

#include <cstdint>
#include <utility>

namespace miniplc0 {

	enum Operation {
	    loada,
        ipush,
        istore,
        cstore,
        iadd,
        isub,
        imul,
        idiv,
        ineg,
        i2c,
        c2i,
        iload,
        jmp,
        cpush,
        iscan,
        cscan,
        iprint,
        cprint,
        sprint,
        loadc,
    };

	class Instruction final {
	private:
		using int32_t = std::int32_t;
	public:
		friend void swap(Instruction& lhs, Instruction& rhs);
	public:
		Instruction(Operation opr, int32_t x, int32_t y) : _opr(opr), _x(x), _y(y) {}
		
		Instruction() : Instruction(Operation::iload, 0,0){}
		Instruction(const Instruction& i) { _opr = i._opr; _x = i._x; _y = i._y;}
		Instruction(Instruction&& i) :Instruction() { swap(*this, i); }
		Instruction& operator=(Instruction i) { swap(*this, i); return *this; }
		bool operator==(const Instruction& i) const { return _opr == i._opr && _x == i._x && _y == i._y; }

		Operation GetOperation() const { return _opr; }
		int32_t GetX() const { return _x; }
        int32_t GetY() const { return _y; }
	private:
		Operation _opr;
		int32_t _x;
        int32_t _y;
	};

	inline void swap(Instruction& lhs, Instruction& rhs) {
		using std::swap;
		swap(lhs._opr, rhs._opr);
		swap(lhs._x, rhs._x);
		swap(lhs._y, rhs._y);
	}
}