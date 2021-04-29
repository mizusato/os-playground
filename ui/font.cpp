#include "font.hpp"

#define PRIMARY_WIDTH  14
#define PRIMARY_HEIGHT 28

class PrimaryFont final: public Font {
public:
    PrimaryFont() {};
    ~PrimaryFont() {};
    Number Width() const override;
    Number Height() const override;
    Byte GetPixel(Char ch, Number x, Number y) const override;
};

Font* GetPrimaryFont() {
    static Byte buf[sizeof(PrimaryFont)];
    static Font* instance = nullptr;
    if (instance == nullptr) {
        instance = new(buf) PrimaryFont;
    }
    return instance;
};

Number PrimaryFont::Width() const { return PRIMARY_WIDTH; }
Number PrimaryFont::Height() const { return PRIMARY_HEIGHT; }

// Refers to the file 'ui/primary.font.bin'
// Copyright Info: converted from "Terminus (TTF) Bold"
extern Byte PrimaryFontData[];

Byte PrimaryFont::GetPixel(Char ch, Number x, Number y) const {
    if ( !(' ' <= ch && ch <= '~') ) {
        return 0xFF;
    }
    Number n = (ch - ' ');
    Number offset = ((PRIMARY_WIDTH * ((n * PRIMARY_HEIGHT) + y)) + x);
    return PrimaryFontData[offset];
}

