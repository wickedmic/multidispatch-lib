# multidispatch-lib
C++ library for multimethods

## Requirements
A C++14 conforming compiler

Tested with gcc 4.9.1 and clang 3.5

## Install
This is a header only library. So the only thing you have to do is to add
the `include` folder to your include path.

## Usage
The central function in this library is `md::dispatch`. It takes a functor
and an abitrary number of parameters. Each parameter can be a simple type
or a handle type. A handle is a **multi-type, single value** type, like for
example `boost::variant` or `md::variant`. A simple type is a non-handle type
and is treated as a handle type with only one type of choice. If you call
`md::dispatch` with one or more such types, it will call the given functor
with the carried types. This way all given objects are available as thier
actual type within the scope of the functor. From there you can use
e.g overload resolution to jump to code of your choice based on the actual
types given.

Here is a small Example:
```cpp
#include <iostream>
#include <utility/multidispatch.hpp>
#include <utility/variant.hpp>
#include <utility/register_variant.hpp>

// catch all 'output'
template<typename T1, typename T2>
bool output(T1& p1, T2& p2)
{
    std::cout << std::boolalpha << "p1: " << p1 << " p2: " << p2 << std::endl;
    return false;
};

// specialized 'output'
bool output(int i, float f)
{
    std::cout << "i: " << i << " f: " << f << std::endl;
    return true;
}


int main()
{
    // define a handle type which can carry an int, float, or boolean
    using var_t = utility::variant<int, float, bool>;

    var_t var1 = 1;
    var_t var2 = 2.3f;

    // decide at runtime which 'output' function is called
    auto result = md::dispatch(
        [](auto& p1, auto& p2){ return output(p1, p2); },
        var1,
        var2);

    std::cout << std::boolalpha << "result: " << result << std::endl;

    return 0;
}
```

## Adapting your own types
The library supplies an interface to enable your own handle types to be used.
For detailed information look into `include/utility/handle_interface.hpp`.
In `include/utility/register_variant.hpp` and `include/utility/register_boost_variant`
you can see how an implementaion of this interface would look like.
