#ifndef FONT_HPP
#define FONT_HPP

#include "types.h"
#include "string.hpp"

class Font {
protected:
    Font() {};
public:
    virtual ~Font() {};
    virtual Number Width() const = 0;
    virtual Number Height() const = 0;
    virtual Byte GetPixel(Char ch, Number x, Number y) const = 0;
};

class BasicFont final: public Font {
public:
    BasicFont() {};
    ~BasicFont() {};
    Number Width() const override;
    Number Height() const override;
    Byte GetPixel(Char ch, Number x, Number y) const override;
    static const Number* GetCharData(Char ch);
};

#endif

