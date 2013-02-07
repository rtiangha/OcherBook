#include "clc/data/Buffer.h"


SUITE(Buffer)
{
    TEST(constructors)
    {
        PRINT_PROGRESS;
        // Trivial
        {
            clc::Buffer a;
            CHECK_EQUAL(a.length(), 0U);
        }

        // C string
        {
            char const* source = "Hello World!";
            clc::Buffer a(source);
            CHECK_EQUAL(strlen(source), (size_t)a.length());
            CHECK      (a.c_str() != source);  // Should have copied
            CHECK      (a == source);
        }

        // Truncated C string
        {
            char const* source = "Hello World!";
            clc::Buffer a(source, 5);
            CHECK_EQUAL(5U, a.length());
            CHECK      (a.c_str() != source);  // Should have copied
            CHECK      (strncmp(a.c_str(), source, 5) == 0);
        }
    }

    TEST(utf8)
    {
        PRINT_PROGRESS;
        // 4 characters:
        //   ampersand     1 byte
        //   cyrillic zhe  2 bytes
        //   chinese       3 bytes
        //   symbol        4 b6ytes
        clc::Buffer a("&\xd0\x96\xe4\xb8\xad\xf0\x90\x8d\x86");
        CHECK_EQUAL(10U, a.length());
        CHECK_EQUAL(4U, a.countChars());
    }

    TEST(format)
    {
        PRINT_PROGRESS;
        clc::Buffer a;
        a.format("Big number: %x", 0x12345678);
        CHECK_EQUAL(20U, a.length());
    }

    TEST(appendFormat)
    {
        PRINT_PROGRESS;
        clc::Buffer a;
        a = "Big number";
        a.appendFormat(": %x", 0x12345678);
        CHECK_EQUAL(20U, a.length());
    }

    TEST(copyOnWrite)
    {
        PRINT_PROGRESS;
        clc::Buffer a("Hello World");
        clc::Buffer b(a);
        b.append(" Domination");
        CHECK_EQUAL("Hello World", a.c_str());
        CHECK_EQUAL("Hello World Domination", b.c_str());
    }

    TEST(c_str)
    {
        PRINT_PROGRESS;
        const char a[] = "multi\nline\nstring\n";
        clc::Buffer b(a);
        CHECK(memcmp(b.c_str(), a, sizeof(a)) == 0);
    }

    TEST(copy)
    {
        PRINT_PROGRESS;
        clc::Buffer* s2;
        clc::Buffer* s4;
        clc::Buffer s5;
        {
            clc::Buffer s1("test123");         // Constructor, on stack
            s2 = new clc::Buffer("test456");   // Constructor, on heap
            clc::Buffer  s3(s1);               // Copy constructor, on stack
            s4 = new clc::Buffer(s3);          // Copy constructor, on heap
            s5 = s3;                          // Assignment, to stack value which is going away
        }
        delete s2;
        delete s4;
        // Should not crash when s5 gets destructed (was shared with now-defunct s3)
        // Should have no leaks
    }

    TEST(binary)
    {
        PRINT_PROGRESS;
        const size_t len = 16;
        const char data[len] = {
            0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x18,
            0x19, 0x00, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
        };
        clc::Buffer s1(data, len);
        CHECK_EQUAL(len, s1.length());
        CHECK(memcmp(s1.c_str(), data, len) == 0);
        // Copies should be exact -- not somehow coerced to a C string
        clc::Buffer s2(s1);
        CHECK_EQUAL(len, s2.length());
        CHECK(memcmp(s2.c_str(), data, len) == 0);
    }

    TEST(compare)
    {
        PRINT_PROGRESS;
        clc::Buffer s1("testing");
        clc::Buffer s2("testing");
        clc::Buffer s3("Testing");
        clc::Buffer s4("tester");
        clc::Buffer s5("test");

        CHECK(s1 == s2);
        CHECK(s1 != s3);
        CHECK(s1.Compare(s2) == 0);
        CHECK(s1.Compare(s2.c_str()) == 0);
        CHECK(s1.Compare(s3) != 0);
        CHECK(s1.ICompare(s2) == 0);
        CHECK(s1.ICompare(s3) == 0);
        CHECK(s1 > s5);
        CHECK(s1.Compare(s5) > 0);
        CHECK(s1.Compare(s5.c_str()) > 0);
        CHECK(s5 < s1);
        CHECK(s5.Compare(s1) < 0);
        CHECK(s5.Compare(s1.c_str()) < 0);
        CHECK(s1.Compare(s4, 4) == 0);
        CHECK(s3.ICompare(s4, 4) == 0);
        CHECK(s1.Compare(s4, 5) != 0);
        CHECK(s3.ICompare(s4, 5) != 0);
    }

    TEST(binaryCompare)
    {
        PRINT_PROGRESS;
        clc::Buffer s1("te\0ting", 7);
        clc::Buffer s2("te\0ting", 7);
        clc::Buffer s3("Te\0ting", 7);
        clc::Buffer s4("te\0ter", 6);
        clc::Buffer s5("te\0t", 4);

        CHECK(s1 == s2);
        CHECK(s1 != s3);
        CHECK(s1.Compare(s2) == 0);
        CHECK(s1.Compare(s2.c_str()) > 0);
        CHECK(s1.Compare(s3) != 0);
        CHECK(s1.ICompare(s2) == 0);
        CHECK(s1.ICompare(s3) == 0);
        CHECK(s1 > s5);
        CHECK(s1.Compare(s5) > 0);
        CHECK(s1.Compare(s5.c_str()) > 0);
        CHECK(s5 < s1);
        CHECK(s5.Compare(s1) < 0);
        CHECK(s5.Compare(s1.c_str()) > 0);
        CHECK(s1.Compare(s4, 4) == 0);
        CHECK(s3.ICompare(s4, 4) == 0);
        CHECK(s1.Compare(s4, 5) != 0);
        CHECK(s3.ICompare(s4, 5) == 0);  // Case implies all treated as C string
    }

    TEST(setToTrunc)
    {
        PRINT_PROGRESS;
        clc::Buffer s;
        s.setTo("testing");
        CHECK(s == "testing");
        s.setTo(s, 4);
        CHECK(s == "test");
    }
}
