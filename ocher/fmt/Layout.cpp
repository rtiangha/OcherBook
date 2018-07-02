/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "fmt/Layout.h"

#include "util/Debug.h"
#include "util/Logger.h"

#include <cctype>

Layout::Layout() :
    m_dataLen(0),
    nl(0),
    ws(0),
    pre(0),
    m_buffer(new Buffer),
    m_bufferLen(0)
{
    m_buffer->lockBuffer(chunk);
    m_data.lockBuffer(chunk);
}

Layout::~Layout()
{
    // Walk the bytecode and delete embedded strings
    const unsigned int N = m_data.size();
    const char* raw = m_data.data();

    for (unsigned int i = 0; i < N; ) {
        auto code = *(const uint16_t*)(raw + i);
        i += 2;
        unsigned int opType = (code >> 12) & 0xf;
        unsigned int op = (code >> 8) & 0xf;
        if (opType == OpCmd && op == CmdOutputStr) {
            delete *(Buffer* const*)(raw + i);
            i += sizeof(Buffer*);
        }
    }

    delete m_buffer;
}

Buffer Layout::unlock()
{
    flushText();
    delete m_buffer;
    m_buffer = nullptr;

    m_data.unlockBuffer(m_dataLen);
    return m_data;
}

char* Layout::checkAlloc(unsigned int n)
{
    if (m_dataLen + n > m_data.size()) {
        m_data.unlockBuffer(m_data.size());
        m_data.lockBuffer(m_dataLen + n + chunk);
    }
    char* p = m_data.c_str() + m_dataLen;
    m_dataLen += n;
    return p;
}

void Layout::push(unsigned int opType, unsigned int op, unsigned int arg)
{
    char* p = checkAlloc(2);
    uint16_t i = (opType << 12) | (op << 8) | arg;

    *(uint16_t*)p = i;
}

void Layout::pushPtr(void* ptr)
{
    int n = sizeof(ptr);
    char* p = checkAlloc(n);

    *((char**)p) = (char*)ptr;
}

void Layout::pushTextAttr(TextAttr attr, uint8_t arg)
{
    push(OpPushTextAttr, attr, arg);
}

void Layout::popTextAttr(unsigned int n)
{
    push(OpCmd, CmdPopAttr, n);
}

void Layout::pushLineAttr(LineAttr attr, uint8_t arg)
{
    push(OpPushLineAttr, attr, arg);
}

void Layout::popLineAttr(unsigned int n)
{
    push(OpCmd, CmdPopAttr, n);
}

// void Layout::pushImage()
// {
//	push(OpImage, );
// }

inline void Layout::_outputChar(char c)
{
    if (m_bufferLen == chunk) {
        flushText();
    }
    (*m_buffer)[m_bufferLen++] = c;
}

void Layout::outputChar(char c)
{
    if (isspace(c)) {
        if (!ws) {
            ws = 1;
            _outputChar(' ');
        }
    } else {
        nl = 0;
        ws = 0;
        _outputChar(c);
    }
}

void Layout::outputNl()
{
    if (!nl) {
        _outputChar('\n');
        nl = 1;
    }
}

void Layout::outputBr()
{
    _outputChar('\n');
    nl = 1;
}

void Layout::flushText()
{
    if (m_bufferLen) {
        push(OpCmd, CmdOutputStr, 0);
        m_buffer->unlockBuffer(m_bufferLen);
        pushPtr(m_buffer);
        // m_buffer pointer is now owned by the layout bytecode.
        m_buffer = new Buffer;
        m_buffer->lockBuffer(chunk);
        m_bufferLen = 0;
    }
}

void Layout::outputPageBreak()
{
    push(OpCmd, CmdForcePage, 0);
}
