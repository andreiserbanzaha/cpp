# C++ fundamentals

- data types
- expressions
- pointers
- references
- lambda expressions
- value categories
- constexpr
- semantics
- variadic templates
- forwarding

## Data types (possible values + operations)
- categories:
    - primitive (char, int, bool, void ...)
    - built-in (lists, hash tables, complex numbers ...)
    - composite (array, struct, class)
    - user defined
    - abstract
    - atomic
    - pointer
    - reference
    - function pointer
    - method pointer
    - lambda expression

## Expressions (data type + value category)
- sequence of operators and their operands which specify some computation process

## Value categories (rvalue and lvalue)
- has identity
    - does it have a name
    - does it have a memory location
    - can you take its address

## Pointers

## References
- const reference **const&**
- lvalue reference **&**
- rvalue reference **&&**

## Lambda expressions
- **[capture clause] (parameter list) -> return type { body };**
- syntax to declare an object which can be invoked
- can be assigned to a variable (auto)
- a lambda expression is only considered a **closure** when there is something in the capture clause
- capture clause
    - C++11:
        - capture by value/reference [var]
    - C++14:
        - capture by value [var = 10]
        - capture by reference [&var = x]
        - capture by move [var = std::move(x)]
- capture *this*:
    - C++11: [this] captures by value
    - C++14: [self = *this] capture *this by value (copy)
    - C++17: [*this] captures *this by value (copy)

- **callback** = function that invokes another function
- **function pointer** = pointer to a function
- **functor** = NOT defined in the C++ standard
- **function object** = a class which declares operator()() method. An instance can be called as if it were a function
- **std::function** = container which holds a single function object

## Semantics
- **value/copy** semantics
- **move** semantics
- **reference/pointer** semantics

## Forwarding
- **forwarding reference** -> auto&& or T&& for templates
- **reference collapsing** -> the compiler has the option to discard one of the & if required to instantiate the template
- **perfect forwarding** -> preserve the argument's data type and value category when passing to another function (std::forward())

## Casting
- **static_cast** (always defined, known at compile time)
- **dynamic_cast** (always defined, might fail at runtime)
- **const_cast** (used to remove const)
- **reinterpret_cast** (shut up compiler cast)
- **(int)** (c style cast -> dangerous cast)

## Smart pointers (since C++11)
- implemented as a templated class
- types: **unique_ptr**, **shared_ptr**, **weak_ptr**, **auto_ptr** (deprecated since C++11)

## Data type deduction
- auto
- decltype()
- declval()

## Undefined behavior
- result of executing code whose behavior is NOT defined by the language specifications
- each operation executed is either *defined* or *undefined*
- C++ standard makes no guarantees on how the code will execute at runtime
- responsibility of the programmer to NOT cause undefined behavior
- a correct program is free of undefined behavior
- user defined code must be implemented *correctly* to NOT have undefined behavior
- undefined behavior is NOT an error
- where it's good/useful:
    - simplifies what the compiler does
    - makes it possible for the compiler to generate very efficient code (e.g. tight loops)
    - allows compiler to optimize for target platform
- defined *undefined behaviors*
    - dereference a null pointer
    - access out of bounds
    - use of an uninitialized variable
    - access to an object using a ptr of a different type
    - use of an object after it has been destroyed
    - infinite loop without side effects
    - race condition
    - shifting more than the width of an integer
    - calling a pure virtual function from a ctor or dtor
    - integer divide by zero
    - signed integer overflow
- how to avoid undefined behavior:
    - pay attention to compiler warnings
    - read the C++ standard
    - run a static analyzer (e.g. coverity, clang static analyzer, purify ...)
    - run sanitizers (UBSan)

## Sanitizers
- **ASan** (clang and gcc)
- **UBSan** (clang and gcc)
- **MSan** (clang)
- **TSan** (clang)
- **Valgrind** (combination of ASan and UBSan)

## Overload Resolution
- applies to any function, constructor or method and most operators
- 2 or more functions are overloaded if:
    - same function name
    - are visibile from the same scope
    - have different set of parameters
- when it's an error:
    - differ only by return type
    - differ only in default arguments
    - same name and parameters but one is static
- it's the process of selecting the most appropriate function to call
    - the compiler must decide which overload to call
        - at compile time
        - only consider the argument data types passed and how they match the parameter data types, NOT the values that are passed
        - if compiler cannot choose -> ambiguous
        - template functions and methods are considered, but if 2 overloads are deemed equal, the non-template will be preferred
- argument conversions:
    - standard conversions (promotion) -> implicit conversions between primitive data types (e.g int -> float)
    - user defined conversions -> implicit conversions between user defined data types
- steps:
    - the compiler creates a list of  candidates
    - remove invalid canditates (NOT viable)
    - rank the candidates
        - exact match
        - promotions (e.g. char -> int)
        - standard conversions (e.g. int -> bool)
        - user defined conversions (e.g. const char* -> std::string)
    - tie breakers -> if best candidates equal (e.g. template vs non-template)

## Type Traits ???
- when are they used:
    - anytime you need to know a specific information about a data type
    - ensure that your integer data type does NOT exceed an upper bound
    - query how many digits a floating point number can represent
    - do something different in your code based on a given data type
        - use routing A for float
        - use routing B for double
        - return error if data type is an int
- why type traits are of value
    - using type traits in a template allows the code to ask questions about the data type and -
        - choose a more efficient algorithm
        - disallow certain data types
        - leverage SFINAE
        - call a specialization
    - type traits provide various attributes about a template parameter
        - can T be copied
        - is T a pointer
        - if T is const, remove the const
        - can copying T throw an exception
- definition of a type trait
    - a type trait exposes the characteristics of a data type
    - retrieves information about a data type at compile time
    - type traits are templated classes/structs -> their only purpose is to store information about the data type
    - normally the internal information is stored as public definitions and there are no data members
    - the default trait struct can be empty or provide default values
    - template specializations provide the specific details
    - you can write your own type trait as long as you are asking a question which can be answered by the existing C++ type system
    - there are some type traits that you cannot implement yourself because it requires compiler internals (e.g. is_enum, is_empty, is_abstract ...)
- examples
    - std::numeric_limits\<long\>::max()
    - std::is_enum\<T\>::value
    - std::is_const\<T\>::value
    - std::remove_reference\<T\>::type
    - etc
- trait categories:
    - numeric_limits\<T\>::?? (max(), min(), is_signed, is_integer, ...)
    - type categories (is_array\<T\>::value, is_class\<T\>::value, ...)
    - type properties (is_const\<T\>::value, is_final\<T\>::value, ...)
    - supported operations (is_constructible\<T, Args\>::value, is_copy_assignable\<T\>::value, ...)
    - relationships (is_same\<T, U\>::value, is_base_of\<T, U\>::value, is_convertible\<T, U\>::value ...)
    - modifications (remove_const\<T\>::type, remove_reference\<T\>::type, add_const\<T\>::type ...)
    - transformations (conditional\<B, T, F\>::type, enable_if\<B, T\>::type, underlying_type\<T\>::type ...)
- type traits enhanced
    - *_t for types (e.g. std::remove_const\<T\>::type -> std::remove_const_t\<T\>)
    - *_v for values (e.g. std::is_enum\<T\>::value -> std::is_enum_v\<T\>)
- policy class
    - defines behavior another template will utilize
    - must be provided as a template parameter
    - e.g. std::map\<Key, Val, Compare, Allocator\>
        - Compare and Allocator are policy classes
- a trait is similar to a policy
    - a policy class is passed as a template parameter
    - a trait is used on the template parameter

## C++ ISO Standard
- ISO = International Organization for Standardization
- languages that have an ISO standard (C, C++, C#, Ada, Algol, Prolog, Basic, Pascal, Cobol, Fortran, Ruby, JavaScript, SQL)
- if a language does NOT have an ISO standard, porting from one compiler to another may produce inconsistent results
- creating an ISO can lead to slower language development

## typedef vs using ???
- declares something called *type alias*
- *using* is preferred -> more powerful since it can be templated (template \<typename T\> using ptr = T*;)
- are done by the compiler
- can improve readability
- common typedefs: containers, iterators, allocators, smart pointers

## MACROs
- literal text substitutions done by the preprocessor
- are defined until the end of the source file
- no scoping rules

## namespace
- organize source code into related groups
- prevent class/function name collisions
- global namespace (begins with ::)

## forward declaration
- declaration - contains enough code to indicate the signature
- definition - includes the class members but may NOT contain the full implementation
- forward declaring std namespace -> undefined behavior
- we can create forward declaration headers -> can change implementation without refactoring

## Typename ???
- keyword used when writing templates
- required in a specific set of places
- NOT allowed to be used unless required
- when a template type parameter, typename == class
- non-type template parameters: integral data types (int, unsigned short, long, char, other integer types), enum, lvalue reference, nullptr_t, pointer, member pointer, method pointer
- template template parameter -> similar to template type parameter, but passed data must be a template
    - e.g.: template \<typename \<typename, typename\> Box, typename Key, typename Value\> Box \<Value, Key\> reverse_map(const Box\<Key, Value\>& box){...}
- when to use:
    - to mark a *dependent name* as data type
    - typename is required in a *using* statement when the data type is a *dependent name*
        - e.g. using value_type = typename std::vector\<T\>::value_type;
    - typename is required in a type trait when it contains a *dependent name*
        - e.g. std::vector\<typename std::remove_reference\<T\>::type\> myVector;
    - using declaration is located inside of a class

## Virtual
- method which exists in a base class
    - *virtual* keyword precedes the declaration
    - may or may NOT have an implementation
- method with exact signature can be redefined in derived classes
    - NOT mandatory
    - *override* should be added -> compiler checks if same exact signature
- allows the method in the derived class to be called even when the method is invoked using a pointer or a reference to the parent class
- if the method is NOT marked *virtual*
    - the derived class cannot override it
    - you can create a method with same signature, but it will be *shadowed*
- if method NOT virtual -> *early binding* -> which implementation will be called is decided at compile time
- if method virtual -> *late binding* -> which implementation will be called is decided at runtime

## Pure virtual ( = 0 )
- no implementation in the base class -> NOT enough information
- a class with at least one pure virtual method = abstract class (cannot be instantiated)

## Template
- utilized at compiled time to generate a class, function, method or variable based on one or more data types
- instantiated at compile time -> all information needs to be available
- can be specialized for specific types
- used at compile time to generate a class, function, method or variable based on one or more data types
- designed without knowing the required data types
- most of the cost of using templates is at compile time
- very powerful for generic programming
- when to use?
    - when multiple variations of the same construct are needed for a variety of different data types
    - vs overloaded functions:
        - if same code regardless of data types -> template
        - if different code based on data types -> overload
-

## Variadic templates (...) ???
- class templates, functions, methods and variables can be variadic
- the ellipsis operator (...) has 2 roles:
    - on the left of the parameter -> declares a *parameter pack* (typename ... Ts)
    - on the right of the parameter -> unpacks the parameter pack into separate arguments (Ts ... Vs)

## SFINAE (substitution failure is NOT an error) ???
- is NOT something you define, it's just a rule you can make use of to when you set up your templates
- during template instantiation (compile time)
- if the compiler is NOT able to deduce a template parameter, then this template specialization is ignored
- if another specialization can be instantiated succesfully, no compile-time error is generated

## keywords
- are reserved and can NOT be redefined or overloaded
- can NOT be used as the name of an object or function
- e.g.: const, constexpr, for, break, template, enum, int, false, void, static, inline etc.

## identifiers
- only have semantics when used in the right context
- can be used as the name of an object or function
- the only identifiers: final, override

## const
- part of a data type
- const objects can never be modified
- some code promises NOT to change something
- where can const appear in a data type:
    - const variable (const int foo)
    - const reference (const Foo& foo)
    - const pointer (Foo* const fooPtr)
    - pointer to const (Foo const* fooPtr)
    - const method (Foo getFoo() const)

## constexpr
- part of a declaration, but NOT part of a data type
- similar to static and inline
- sometimes adds const as side effect
- where constexpr can be used:
    - variable declaration (constexpr int foo)
    - function declaration (constexpr int myFunction())
    - class method declaration (constexpr int myMethod())
    - constructor declaration (constexpr Foo())
    - lambda (since C++17)
- usable data types in a constexpr declaration:
    - literal data types:
        - scalar types: int, long, double, pointer, enum etc.
        - any kind of reference: lvalue reference, const reference, rvalue reference
        - an array of literal types
        - void (since C++14)
        - class:
            - constexpr constructor which is NOT move or copy
            - trivial destructor (defaulted or NOT declared)
            - data members or any base classes (must be literal type and NOT volatile)
- what does constexpr mean in a declaration:
    - asks the compiler to evaluate and run the code at compile-time
    - compiler is NOT always required to do the work at compile-time
    - usually it will, but there is no guarantee or way to check
    - variable declaration:
        - variable type must be a literal data type
        - data type of the variable becomes const
        - value must be initialized at compile time
        - eg: constexpr int arraySize = 42;
    - function, method, or constructor declaration
        - all variables must be a literal data type
- constant expression
    - constexpr is NOT a constant expression
    - constant expression = value which can be used as a compile-time constant
    - constexpr variable
        - adds the ability to save a compile-time constant in a variable
        - a constexpr variable declaration must be initialized with a constant expression
- a **const int** can be used at compile-time since C++98
- how constexpr can imply const:
    - affects the data type of the variable -> implicit const
    - does NOT affect the signature of the function
    - does NOT affect signature of a constructor
- constexpr vs macros
    - constexpr variables and functions respect scoping, whereas #define macro is a text substitution
    - replacing macros with constexpr:
        - improve readability
        - enforces usage of data types
        - move work from preprocessor to compiler
    - constexpr is NOT a replacement for all macros
    - use macros when the value needs to be understood by the preprocessor (e.g. #if)
- constexpr static
- constexpr static const

## Containers
- definition
    - stores a collection of elements
    - each element must be the same data type
    - a container manages the storage space for its elements (normall delegated to an Allocator)
    - implemented as a class template (instantiated at compile-time with the data type used)
    - provides member functions/methods to access the elements
        - directly, using an index or a key
        - using iterators
    - constructors
        - create an empty container
        - copy construct and move construct
    - insert or append
    - remove single element or range of elements
    - erase all elements
    - return the number of elements
    - provide access to an element
    - search, find
- categories
    - sequential containers
        - user controls the order of the elements
        - position depends on where and when the element was added
        - e.g. vector, list, linked lists, deque, queue, stack)
    - associative
        - container controls the position of the elements
        - value of an element determines its position
        - e.g. map, multimap, hash, multihash, flatmap, set
    - container adaptors
        - a container implemented using another container
        - acts as a wrapper to limit functionality or provide a more convenient API
        - default container can be replaced by passing a template parameter
        - e.g.  std::stack -> std::deque, std::queue -> std::deque, std::priority_queue -> std::vector
- how to store in a container
    - by value (preferred) -> usually more efficient
    - pointer to the actual data
        - your responsibility to ensure data lifetime
        - smart pointer is preferred
- which is best depends on
    - functionality
    - performance
- algorithms
    - STL containers were designed to be similar so that standard functions (algorithms) can work with almost any container
    - do NOT take containers as an argument, instead you pass a begin() and end() iterator to define a range
    - templated functions
    - std::binary_search, std::equal_range, std::sort, std::is_sorted, std::rotate, std::lower_bound etc
- why the implementation matters
    - STL has container classes approved by the standard committee
    - various libraries provide their own version of containers
    - what should you look for when choosing an implementation
        - do the containers conform to the C++ standard?
        - ones which are just fast?
    - C++ standard requirements:
        - methods and operations supported by a given container
        - semantics: iterator invalidation, speed, move, how to handle exceptions etc.
- custom containers
    - why no?
        - what signatures will you support? (does size() return int or size_type ?)
        - what is your support for typedefs ? (std::random_access_iterator_tag ?)
        - supporting move semantics properly is NOT easy (std::move_if_nothrow?)
        - specialization for certain data types
        - users learn new API
        - testing every method is painful
        - testing all corner cases is time consuming
        - exception safety
        - etc.
    - why yes?
        - support an extended API
        - allow legacy data structures to be used with STL algorithms
        - data structure needed does NOT exist in STL (b-tree, flatmap, radix tree etc.)
        - store data in an unusual way

### Copy on write
- defer copy only when container is modified
- pitfals
    - every other modifying operation is slower
    - unusual iterator behavior
        - make copy of a container
        - use a const interator to walk the original container
        - remove certain elements
        - if dereferencing the const iterator -> original elements
- as of C++11
    - move semantics were added to prevent most copies (no more reason for copy on write)
    - does NOT allow copy-on-write for any STL container
        - operator[] is NOT allowed to invalidate iterators
        - making a copy of the container invalidates iterators
        - in a copy-on-write contaier, operator[] may need to copy

## Multithreading
- is the ability of a program to run multiple instructions at the same time
    - a mechanism by which a single set of code can be used by several threads at different stages of execution
    - the ability to execute different parts of a program simultaneously
    - multithreading may be considered:
        - *concurrency* if the threads interact
        - *parallelism* if they not
- not the solution to everything
- problems for which multithreading is the answer:
    - tasks which can intuitively be split into independent processing steps
    - a problem where each step has a clear input and output
    - intensive computations
    - continuous access to a large read-only data set
    - processing a stream of large data files
- problems for which multithreading is the ONLY solution
    - tasks whose performance would be unacceptable as a single thread
    - processes where the workload cannot be anticipated
    - manage concurrent access to multiple resources, such as an operating system
    - external clients sending requests to a process in a random and unpredictable fashion (e.g. PostgreSQL)
- misc advice
    - too many active threads
        - one active thread per core is ideal
        - move blocking calls to extra threads which can then wait, without stalling the rest of the program
    - too much shared data
        - reduce the number of shared resources
        - reduce the size of each shared data structure
        - *reduction of shared data should drive entire design*
        - read-only shared data is much better than writable shared data

## Multitasking
- concept of performing multiple tasks or processes over a certain period of time by executing them concurrenly
- does not automatically imply multithreading
- on a single processor system multitasking is implemented by time slicing and the CPU switches between different tasks

## Thread
- work which can be scheduled to execute on one core
- a thread is contained inside a process
- each thread has its own call stack

## Process
- used to start a separate program
- if there is only one thread in a process -> not multithreaded
- threads in the same process can share most resources

## Core
- **core count** is the total number of instructions that can be executed simultaneously
- a computer may have multiple processors, each of which might have multiple cores
- a thread consumes an entire core while it's active
- more cores does not mean program will run faster
- not all cores are equal (HyperThreading, NUMA, AMP)

## Resource
- e.g.
    - computer memory location
    - file handle
    - non thread-safe C++ objects
- a resource must NOT be accessed by multiple threads simultaneously

## Race condition
- occurs when a resource is accessed by multiple threads simultaneously, and at least one is a *write*
- undefined behavior

## Data race
- can happen in a multi-threaded program
- a multi-threaded program has a data race if all the following are true:
    - multiple threads access the same memory location
    - at least one is a write
    - memory operations are not ordered by "do X before Y" (heppens-before relationship)

## Stack
- in an area of memory used for data whose size is determined at compile-time
- belongs to a specific thread

## Heap
- an area of memory used for data which is allocated at run-time
- shared among all threads

## Thread
- every program has at least one thread (main)
- multiple threads can only exist within a program
- nothing unique about the functionality executed by a given thread
- threads can:
    - execute concurrently but independently
    - each thread has its own callstack
    - share resources such as memory
    - share the values of global data
- value of threads:
    - main thread of a program is started by the operating system
    - there is a *main thread* even if you are not aware of it
    - can increase the speed of a program
    - allow more efficient use of resources
    - can provide a better breakdown of tasks
- a multi-threaded program is one where a multiple threads are explicitly started
- each thread will run independently of the main thread

### C++ threads
- in standard since C++11
- every thread has unique id
- can catch an exception
- exceptions can be thrown if they are caught in the same thread
- if an exception is thrown and not caught -> std::terminate()
- if you do not wait for all threads to finish, it's possible for main thread will finish first and the program will exit -> all threads that were not finished will be terminated

## Simultaneous execution

### Parallelism
- several processes are performing independent operations
- no process has to consider what others are doing

### Concurrency
- several processes are running at the same time
- each process is often accessing the same data set
- processes not guaranteed to be independent

## C++ Memory Model
- added a memory model to the core language in C++11
- provides guarantee regarding the semantics of accessing memory
    - there is now a meaning to how a program will access threads from different threads
- a multi-threaded program must be free of data races
    - this memory model defines what constitutes a data race
    - it is you responsibility to ensure there are no race conditions
    - if you have a data race, your program is deemed to be broken

### C++ Memory Model (before C++11)
- compilers were:
    - allowed to muck with the ordering of memory access
    - there was no unified system to inform the compiler when the order of the operations was of value
- no language requirements
    - when a memory write would occur
    - no guarantee other threads will see the modified data

### C++ threading library abstraction
- main thread starts
- new threads can be created and parameters can be passed
- when a thread has completed, it will automatically terminate
- a program will terminte when all threads, including main, have completed
- a new thread must be *joined* or *detached* with the main thread
    - join() -> main thread must wait for it to finish
    - detach() -> the main thread will never know when it finishes
- if you leave the scope where the thread was started without doing one of these, the destructor will terminate your program
- threads will not run in a repeatable or predictable order

## Atomic data types
- abstraction for simultaneous access to a single memory location
- protects a value so access is guaranteed not to cause a data race
- can be used to synchronize memory operations from different threads without explicit locking (e.g. mutex)
- using atomics does not guarantee your code is free from potential logic problems but it does eliminate all data races
- TO-DO: look over compare_exchange_weak ???

## Futures and promises
- a way to pass values between threads without needing synchronization, such as locking a mutex
- when to use:
    - reading or writing data (large files from disk, web service calls over HTTPS, reading data from socket etc)
    - database queries
    - computation which is time intensive
    - responsive user interface
    - distributed systems
- futures
- promises
- how they work
- async
- examples