#ifndef FONT_HPP
#define FONT_HPP

#include "types.h"

class Font {
protected:
    Font() {};
public:
    virtual ~Font() {};
    virtual Number Width() const = 0;
    virtual Number Height() const = 0;
    struct Pixel { Byte lightness; Byte alpha; };
    virtual Pixel GetPixel(Char ch, Number x, Number y) const = 0;
};

class BasicFont final: public Font {
public:
    BasicFont() {};
    ~BasicFont() {};
    Number Width() const override;
    Number Height() const override;
    Pixel GetPixel(Char ch, Number x, Number y) const override;
    static const Number* GetCharData(Char ch);
};

#endif

