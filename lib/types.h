#ifndef OS_TYPES_H
#define OS_TYPES_H

typedef void            Void;

typedef signed char     Int8;
typedef unsigned char   Uint8;
typedef Uint8           Byte;

typedef signed short    Int16;
typedef unsigned short  Uint16;
typedef Uint16          Word;

typedef signed int      Int32;
typedef unsigned int    Uint32;
typedef Uint32          Dword;

typedef Int32           Int;
typedef Uint32          Size;

typedef Int Bool;
#define False 0
#define True  1

#endif  // OS_TYPES_H
