
struct X
{
    void foo(int, std::string const&);
    std::string bar(std::string const&);
};

X x;
auto f1 = std::async(&X::foo, &x, 42, "hello"); // calls p->foo(42, "hello") where p is &x
auto f2 = std::async(&X::bar, x, "goodbye"); // calls tmpx.bar("goodbye") // where tmpx is a copy of x

struct Y
{
    double operator()(double);
};

Y y;
auto f3 = std::async(Y(), 3.14); // calls tmpy(3.14) where tmpy is move-constructed from Y() because it is rvalue
auto f4 = std::async(std::ref(y), 3.14); // calls y(3.14)

X baz(X&);
std::async(baz, std::ref(x); // calls baz(x);

auto f5 = std::async(MoveConstructedClass{});
// calls tmp() where tmp is constructed from std::move(MoveConstructedClass)

auto f6 = std::async(std::launch::async, Y(), 1.2); // run in new thread
auto f7 = std::async(std::launch::deferred, baz, std::ref(x)); // run in wait() or get()
f7.wait(); // invoke deferred function

auto f8 = std::async(std::launch::deferred | std::launch::async, baz, std::ref(x)); // implementation chooses
auto f9 = std::async(baz, std::ref(x)); // implementation chooses


