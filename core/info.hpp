#ifndef INFO_HPP
#define INFO_HPP

#include "heap.hpp"
#include "string.hpp"

String GetMemoryInfo() {
    static const char* MemoryKindNames[] = { MEMORY_KIND_NAMES };
    auto efiMemInfo = Heap::GetEfiMemoryInfo();
    auto status = Heap::GetStatus();
    Number stub = 0;
    String::Builder buf;
    buf.Write("*** Stack Stub Address ***\n");
    buf.Write(String::ReadableSize((Number) &stub));
    buf.Write("\n");
    buf.Write("*** EFI Memory Map ***\n");
    buf.Write("addr / size / kind\n");
    Number base = (Number) efiMemInfo->mapBuffer;
    for (Number ptr = base; (ptr - base) < efiMemInfo->mapSize; ptr += efiMemInfo->descSize) {
        MemoryDescriptor* desc = reinterpret_cast<MemoryDescriptor*>(ptr);
        if (desc->physicalStart < 0x100000) {
            continue;
        }
        const char* kindName = MemoryKindNames[desc->kind.raw];
        buf.Write(String::ReadableSize((Number) desc->physicalStart));
        buf.Write(" / ");
        buf.Write(String::ReadableSize((Number) (desc->numberOfPages * 4096)));
        buf.Write(" / ");
        buf.Write(kindName);
        buf.Write("\n");
    }
    buf.Write("*** Consumed as Static ***\n");
    buf.Write(String::ReadableSize(status.StaticPosition));
    buf.Write(" (");
    buf.Write(String::ReadableSize(status.StaticSize));
    buf.Write(")\n");
    buf.Write("*** Consumed as Chunks ***\n");
    Number n;
    const HeapMemoryInfo* info = Heap::GetInfo(&n);
    for (Number i = 0; i < n; i += 1) {
        buf.Write(String::ReadableSize(info[i].start));
        buf.Write(" (");
        buf.Write(String::ReadableSize(info[i].size));
        buf.Write(")\n");
    }
    return buf.Collect();
}

#endif

