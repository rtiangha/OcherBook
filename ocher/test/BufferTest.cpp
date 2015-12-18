#include "ocher/util/Buffer.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


TEST_CASE("Buffer constructors") {
    char const *source = "Hello World!";

    SECTION("Trivial") {
        Buffer a;

        CHECK(a.length() == 0U);
    }

    SECTION("C string") {
        Buffer a(source);

        CHECK(strlen(source) == (size_t)a.length());
        CHECK(a.c_str() != source);  // Should have copied
        CHECK(a == source);
    }

    SECTION("Truncated C string") {
        Buffer a(source, 5);

        CHECK(5U == a.length());
        CHECK(a.c_str() != source);  // Should have copied
        CHECK(strncmp(a.c_str(), source, 5) == 0);
    }
}

TEST_CASE("Buffer utf8") {
    // 4 characters:
    //   ampersand     1 byte
    //   cyrillic zhe  2 bytes
    //   chinese       3 bytes
    //   symbol        4 bytes
    Buffer a("&\xd0\x96\xe4\xb8\xad\xf0\x90\x8d\x86");

    CHECK(10U == a.length());
}

TEST_CASE("Buffer format") {
    Buffer a;

    a.format("Big number: %x", 0x12345678);
    CHECK(20U == a.length());
}

TEST_CASE("Buffer appendFormat") {
    Buffer a;

    a = "Big number";
    a.appendFormat(": %x", 0x12345678);
    CHECK(20U == a.length());
}

TEST_CASE("Buffer copyOnWrite") {
    Buffer a("Hello World");
    Buffer b(a);

    b.append(" Domination");
    CHECK(a == Buffer("Hello World"));
    CHECK(b == Buffer("Hello World Domination"));
}

TEST_CASE("Buffer c_str") {
    const char a[] = "multi\nline\nstring\n";
    Buffer b(a);

    CHECK(memcmp(b.c_str(), a, sizeof(a)) == 0);
}

TEST_CASE("Buffer copy") {
    Buffer *s2;
    Buffer *s4;
    Buffer s5;
    {
        Buffer s1("test123");         // Constructor, on stack
        s2 = new Buffer("test456");   // Constructor, on heap
        Buffer s3(s1);                // Copy constructor, on stack
        s4 = new Buffer(s3);          // Copy constructor, on heap
        s5 = s3;                      // Assignment, to stack value which is going away
    }

    delete s2;
    delete s4;
    // Should not crash when s5 gets destructed (was shared with now-defunct s3)
    // Should have no leaks
}

TEST_CASE("Buffer binary") {
    const size_t len = 16;
    const char data[len] = {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x18,
        0x19, 0x00, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    Buffer s1(data, len);

    CHECK(s1.length() == len);
    CHECK(memcmp(s1.c_str(), data, len) == 0);
    // Copies should be exact -- not somehow coerced to a C string
    Buffer s2(s1);
    CHECK(s2.length() == len);
    CHECK(memcmp(s2.c_str(), data, len) == 0);
}

TEST_CASE("Buffer compare") {
    Buffer s1("testing");
    Buffer s2("testing");
    Buffer s3("Testing");
    Buffer s4("tester");
    Buffer s5("test");

    CHECK(s1 == s2);
    CHECK(s1 != s3);
    CHECK(s1.compare(s2) == 0);
    CHECK(s1.compare(s2.c_str()) == 0);
    CHECK(s1.compare(s3) != 0);
    CHECK(s1.compareI(s2) == 0);
    CHECK(s1.compareI(s3) == 0);
    CHECK(s1 > s5);
    CHECK(s1.compare(s5) > 0);
    CHECK(s1.compare(s5.c_str()) > 0);
    CHECK(s5 < s1);
    CHECK(s5.compare(s1) < 0);
    CHECK(s5.compare(s1.c_str()) < 0);
    CHECK(s1.compare(s4, 4) == 0);
    CHECK(s3.compareI(s4, 4) == 0);
    CHECK(s1.compare(s4, 5) != 0);
    CHECK(s3.compareI(s4, 5) != 0);
}

TEST_CASE("Buffer binarycompare") {
    Buffer s1("te\0ting", 7);
    Buffer s2("te\0ting", 7);
    Buffer s3("Te\0ting", 7);
    Buffer s4("te\0ter", 6);
    Buffer s5("te\0t", 4);

    CHECK(s1 == s2);
    CHECK(s1 != s3);
    CHECK(s1.compare(s2) == 0);
    CHECK(s1.compare(s2.c_str()) > 0);
    CHECK(s1.compare(s3) != 0);
    CHECK(s1.compareI(s2) == 0);
    CHECK(s1.compareI(s3) == 0);
    CHECK(s1 > s5);
    CHECK(s1.compare(s5) > 0);
    CHECK(s1.compare(s5.c_str()) > 0);
    CHECK(s5 < s1);
    CHECK(s5.compare(s1) < 0);
    CHECK(s5.compare(s1.c_str()) > 0);
    CHECK(s1.compare(s4, 4) == 0);
    CHECK(s3.compareI(s4, 4) == 0);
    CHECK(s1.compare(s4, 5) != 0);
    CHECK(s3.compareI(s4, 5) == 0);  // Case implies all treated as C string
}

TEST_CASE("Buffer setToTrunc") {
    Buffer s;

    s.setTo("testing");
    CHECK(s == Buffer("testing"));
    s.setTo(s, 4);
    CHECK(s == Buffer("test"));
}
