/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "fmt/Layout.h"

#include "util/Debug.h"
#include "util/Logger.h"

#include <cctype>

Layout::Layout() :
    m_buffer(new std::string)
{
    m_buffer->reserve(chunk);
    m_data.reserve(chunk);
}

Layout::~Layout()
{
    // Walk the bytecode and delete embedded strings
    const unsigned int N = m_data.size();
    const unsigned char* raw = m_data.data();

    for (unsigned int i = 0; i < N; ) {
        auto code = *(const uint16_t*)(raw + i);
        i += 2;
        unsigned int opType = (code >> 12) & 0xf;
        unsigned int op = (code >> 8) & 0xf;
        if (opType == OpCmd && op == CmdOutputStr) {
            delete *(std::string* const*)(raw + i);
            i += sizeof(std::string*);
        }
    }

    delete m_buffer;
}

void Layout::finish()
{
    flushText();
    delete m_buffer;
    m_buffer = nullptr;

    m_data.shrink_to_fit();
}

void Layout::push(unsigned int opType, unsigned int op, unsigned int arg)
{
    m_data.push_back(arg);
    m_data.push_back((opType << 4) | op);
}

void Layout::pushPtr(void* ptr)
{
    m_data.insert(m_data.end(), sizeof(char*), 0);
    *((char**)(m_data.data() + m_data.size() - sizeof(char*))) = (char*)ptr;
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
    if (m_buffer->size() == m_buffer->capacity()) {
        flushText();
    }
    m_buffer->push_back(c);
}

void Layout::outputChar(char c)
{
    if (isspace(c)) {
        if (!ws) {
            ws = true;
            _outputChar(' ');
        }
    } else {
        nl = false;
        ws = false;
        _outputChar(c);
    }
}

void Layout::outputNl()
{
    if (!nl) {
        _outputChar('\n');
        nl = true;
    }
}

void Layout::outputBr()
{
    _outputChar('\n');
    nl = true;
}

void Layout::flushText()
{
    if (m_buffer->size()) {
        push(OpCmd, CmdOutputStr, 0);
        m_buffer->shrink_to_fit();
        pushPtr(m_buffer);
        // m_buffer pointer is now owned by the layout bytecode.
        m_buffer = new std::string;
        m_buffer->reserve(chunk);
    }
}

void Layout::outputPageBreak()
{
    flushText();
    push(OpCmd, CmdForcePage, 0);
}
