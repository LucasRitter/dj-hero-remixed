//
// Created by lucas on 2020-12-30.
//

#pragma once


enum EMemoryBlockType {
    FRE,
    DEL,
    FSG,
    STA,
    INV,
    MAX
};

struct MemoryEntry {
    // 0x0
    unsigned char m_cBlockType[4];

    int m_uUnalignedLength;

    unsigned int m_uDataLength;
    void * m_pBlockStart;

    class CHeap * m_pHeap;

    MemoryEntry * m_pNext;
    MemoryEntry * m_pPrevious;
    int m_iField0x1c;
};
