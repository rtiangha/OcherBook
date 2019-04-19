#include <json.hpp>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using nlohmann::json;

// These tests are to test my understanding of how nlohmann::json works.
// They are also acceptance tests of a new nlohmann::json version.
// They are not tests of ocherbook.

TEST_CASE("nlohmann::json unparsable") {
    json j;

    std::stringstream ss("}}}{{{ totally unparsable non-json crap:::\"\"\"");
    REQUIRE_THROWS_AS(j << ss, std::invalid_argument);
}

TEST_CASE("nlohmann::json not found") {
    json j;
    int i;

    i = 1;
    j["foo"] = i;

    // clearly, can get an int:
    i = j["foo"];
    (void)i;

    // trying to get an int (but is null) throws
    REQUIRE_THROWS_AS(i = j["bar"], std::domain_error);
}

TEST_CASE("nlohmann::json no type coercion string to int") {
    json j;
    int i;

    // trying to get an int (but is string) throws
    j["foo"] = "foo";
    REQUIRE_THROWS_AS(i = j["foo"], std::domain_error);

    // trying to get an int (but is possibly coerciable string) throws
    j["foo"] = "42";
    REQUIRE_THROWS_AS(i = j["foo"], std::domain_error);

    (void)i;
}

TEST_CASE("nlohmann::json user mistypes int as string") {
    json j;
    int i;

    j["foo"] = "42";
    try {
        i = j["foo"];
        CHECK(false);  // did throw
    } catch (std::domain_error&) {
        std::string s = j["foo"];
        i = atoi(s.c_str());
        CHECK(i == 42);
    }
}

TEST_CASE("nlohmann::json constructor") {
    // Constructor does not work how I expected.  If I pass a json to the constructor, or assign
    // it, accesses throw the exception:
    //    cannot use operator[] with string
#if 0
    // FAILS
    json j("{ \"key\": \"garbage\" }");
#elif 0
    // FAILS
    json j;
    j = "{ \"key\": \"garbage\" }";
#elif 1
    // OK
    json j;
    std::stringstream ss("{ \"key\": \"garbage\" }");
    j << ss;
#else
    // OK
    json j;
    j["key"] = "garbage";
#endif

    std::string v;

    v = j["key"];
    CHECK(v == "garbage");

    v = j.value("key", "default-value");
    CHECK(v == "garbage");

    v = j.value("key", std::string("default-value"));
    CHECK(v == "garbage");

    std::string key("key");
    std::string defaultValue("default-value");
    v = j[key];
    CHECK(v == "garbage");
}
