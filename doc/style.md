# Targets

POSIX and similar free operating systems.  (Currently value small efficient
code more than huge frameworks.)


# C++ Features

C++11, at most.

Use exceptions only for truly exceptional cases.  Out of memory and bugs are
unexpected.  Expect and handle the rest locally.


# Formatting

`clang-format -style=WebKit`

Admittedly sometimes this thing runs off the rails.  Read the [WebKit style
guide](https://webkit.org/code-style-guidelines/) to understand the intent.
Clang is a tool, not the law.

# Documentation

Doxygen comments using `/** */` form, Javadoc style, in headers.

Minimal commenting in C files.  Block comments as necessary to explain why, but
the what should be obvious (if not, rewrite).

Documentation outside the code can be plain text, but prefer "github flavored
markdown".  This allows useful docs to be uploaded to GitHub easily.  Wrap docs
to 80 columns.

https://help.github.com/categories/writing-on-github/


# Naming

Naming scheme follows Java standard: `MyClass, myMethod, MY_DEFINE,
MyEnum, m_privateMember, MyCFunc, ...`


# Debugging

`ASSERT` contract; minimal or no runtime code to enforce it.  Fail early, fail
hard.

No `printf`s; use logging which can be compiled out.


# Headers

System headers are `<>`, local headers are `""`.

Includes should be ordered "most local first", to more quickly expose missing
includes.

Include guards should be canonicalized form of the path.  Internal to the
header, never external.


# Testing

Strive for good unit test coverage.

Valgrind should be silent.

Write good error handling.  libfiu should never be able to trigger a crash in
OcherBook code.
