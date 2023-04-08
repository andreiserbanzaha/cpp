# Chapter 1: Hello, world of concurrency!

- Concurrency is about two or more activities happening at the same time.
- single system performing multiple independent activities in parallel, rather
    than sequentially
- Hardware concurrency -> when the system is capable of running tasks in
    parallel, not only by using context switching (task switching)
- Context switch 
    - OS has to save the CPU state and instruction pointer for the currently
        running task
    - work out which task to do
    - load the CPU state for the for the task being switched to
    - this takes time!!
    - CPU potentially have to load memory for the instructions and data for the
        new task into cache -> prevent CPU from running any instructions -> even
        further delays

- Some processors can execute multiple threads on a single core.
- Consider the number of hardware threads (how many independent tasks the
    hardware can genuinely run concurrently)

- 2 ways of writing a application using concurrency:
    - concurrency with multiple processes
    - concurrency with multiple threads

- Concurrency with multiple processes
    - divide app into multiple, separate, single-threaded processes that run at
        the same time
    - these processes can communicate with each other through normal
        inter-process communication channels (signals, sockets, files, pipes etc)
    - this kind of communication is hard to set up, slow, or both because
        Operating Systems provide a lot of protection between processes so
        that one does not interfere with another
    - easier to write safe concurrent code

- Concurrency using multiple threads
    - lightweight processes
    - low overhead
    - each thread runs independently from the other
    - each thread runs a different sequence of instructions
    - all threads share the same address space
    - most data can be accessed directly from all threads
    - global variables remain global
    - pointers and refs to object or data can be passed around among threads

- Favored approach of concurrency in mainstream languages (C++ included) is
    using threads because of lower overhead

- Applications that use multiple processes rely on platform-specific API-s.

- Why use concurrency ?
    - Separation of concerns
    - Performance

- Using concurrency for separation of concerns:
    - group related bits of code together
    - keep unrelated bits of code apart
    - easier to understand and test
    - less likely to contain bugs
    - separate distinct areas of functionality, even when the operations in
        these distinct areas need to happen at the same time

- Using concurrency for performance:
    - increased computing power comes not from running a single task faster, but
        from running multiple tasks in parallel
    - 2 ways to use concurrency for performance:
        - task parallelism / data parallelism (each thread -> same operation on
            different data)
        - use the available parallelism to solve bigger problems (e.g. Rather
            than processing one file a a time, process 10 or 20 or more (also
            data parallelism))

- When not to use concurrency?
    - when benefit is not worth the cost (performance gain may not be as large
        as expected, can even be worse)

- Other notes:
    - threads are a limited resource
    - too many threads running at once -> consumes OS resources -> may make the
        whole system run slower
    - too many threads can exhaust the available memory or address space for a
        process -> each thread requires a separate stack space
    - more threads -> more context switch -> takes time


Concurrency and multi-threading in C++:

- Platform specific facilities
    - C++ thread library offers native_handle() member function -> allows the
        underlying implementation to be directly manipulated using
        platform-specific API

- Headers:
    - "<thread>" -> functions and classes for managing threads 
    - other headers -> protecting shared data

- Every thread has to have an initial function, where the new thread execution
    begins.

- For the initial thread in an application -> main()

- For any other thread, it's specified in the constructor of an std::thread object


# Chapter 2: Managing Threads

- when the specified entry point of the function returns -> the thread exits
- starting a thread in C++ always boils down to using std::thread
- creating a thread from a function object -> object is copied into the newly
    created thread and invoked from there -> copy must behave just like
    the original

- C++'s most vexing parse -> passing a temporary variable -> will be interpreted
    as a function declaration
    - e.g. std::thread my_thread(task());
        - std::thread -> return value
        - my_thread -> function name
        - task() -> pointer to a function with no parameters and returning a
            "task" object
    - Avoid by using another set of parentheses or by using uniform
        initialization syntax
 
- join() or detach() must be called before std::thread object is destroyed

- detach() a thread, make sure it does not access objects that are destroyed
    - copy all object that could be destroyed -> self contained
    - usually done immediately after the thread is created (avoid future problems)
    - thread runs in the background 
    - no way to communicating with it
    - not possible to wait for the thread to complete
    - no way to obtain a std::thread reference to it => can no longer be joined
    - ownership and control are passed over to the C++ Runtime Library
        -> assures that the resources associated with it are correctly reclaimed
        when the thread exits
    - daemon threads / processes -> run in the background without any explicit
        user interface
    - typically lung-running (lifetime of the application e.g. monitor file
        system, clearing unused entries from object caches, optimize data structures)
    - "fire and forget" task
    - std::thread object that created it is no longer associated with it
    - thread no longer joinable
    - avoid implicit conversions !! -> when detaching, make sure no implicit
        conversion happens when parameter passed to thread -> could risk of not
        being implicitly converted in time, before function exits -> undefined
        behavior 
        - convert before passing value to thread 

- join() - simple and brute force
    - cleans up storage associated with the thread -> std::thread is not
        associated with any thread anymore
    - can only be called once -> std::thread is no longer joinable() -> will
        return false
    - must be careful when it's called, in case an exception is thrown after its
        creation and before calling join() => catch exception and join() the
        thread there

```
void Foo()
{
    std::thread t{ CallableObject{} }; // create thread

    try
    {
        do_something();
    }
    catch(...)
    {
        t.join(); // join thread in case of exception is thrown
        throw;
    }
    t.join(); // join in case no exception is thrown
}
```

another (better) solution -> RAII

```
class ThreadGuard
{
public:
    explicit ThreadGuard(std::thread& t)
        : m_thread{t}
    {}
    ~ThreadGuard()
    {
        if(m_thread.joinable())
        }
            m_thread.join();
        }
    }
    ThreadGuard(ThreadGuard const&) = delete;
    ThreadGuard& operator=(ThreadGuard const&) = delete;

private:
    std::thread& m_thread;
};
```

- This way, even in case of an exception, the thread will be joined when the
    parent function goes out of scope

- Use std::ref() to pass a reference into a thread's constructor 
- std::thread are moveable but NOT copyable
- one object is associated with one thread of execution

- cannot assign a a std::thread to a std::thread that already has a thread of
    execution assigned to it ! (std::terminate() will be called)

```
std::thread t1{f1};
t1 = std::thread{f2}; // std::terminate()
```

- Oversubscription -> running more threads than hardware can support
    - because of context switching -> more threads will decrease performance

- a thread cannot return a value directly -> reference must be passed and
    modified
- each thread has a unique identified

# Chapter 3: Sharing data between threads

- one of the key benefits of using threads for concurrency -> easily and
    directly share data between them

- Problems with sharing data between threads:
    - all shared data -> read only -> no problem -> data read from one thread is
        NOT affected by another thread reading the same data
    - if at least one thread modifies the data -> problem !
    - invariants:
        - statements that are always true about a particular data structure
        - often broken after an update
        - if one thread modifies an 'invariant' -> invariant broken!


- Race conditions / data race
    - depends on order of execution from different threads
    - can lead to undefined behavior
    - timing sensitive
    - hard to find -> can disappear entirely when running with debugger
        (debugger affects the timing of the program)

- avoiding problematic race conditions
    - wrap the data structure with a protection mechanism -> ensure that only
        the thread performing modifications can see the intermediate states
        where the invariants are broken. From the point of view of other threads
        accessing that data structure, such modifications either haven't started
        or have completed
    - another way to avoid data races -> modify the design of the data structure
        and its invariants so that modifications are done as a series of
        indivisible changes, each of which preserves the invariants -> lock-free
        programming
    - handle the updates to the data structure as a transaction. Modifications
        and reads -> one transaction -> commited in a single step. If the commit
        can't proceed because the data was modified by another thread -> restart
        the transaction -> software transactional memory (STM)


- protecting shared data with mutexes
    - mutually exclusive access to the data structure
    - if one thread is modifying the data structure, the others have to wait
        until it's finished
    - impossible for a thread to see a broken invariant (besides the thread
        currently modifying it)
    - solution: std::mutex (mutual exclusion)
        - before accessing a shared data structure -> lock() the mutex
            associated with that data
        - after finishing accessing the data structure -> unlock() the mutex 
        - the Thread Library ensures that once one thread has locked a specific
            mutex, all other threads that try to lock the same mutex have to
            wait until the thread that has successfully locked the mutex is
            finished and unlocks it
        - problems: deadlocks, protecting too much or too little data

- std::lock_guard is recommended because of RAII
- associated one mutex with data
- any code that has access to the pointer or reference of the data can
    potentially modify it without locking the mutex ! -> careful interface
    design so that this does NOT happen and check that the mutex is locked
    before data access
- functions can also get a handle to the protected data and be used outside of
    the class without the protection of a mutex

- "Don't pass pointers and references to the protected data outside the scope of
    the lock, whether by returning them from a function, storing them in
    externally visible memory, or passing them as arguments to user-supplied
    functions"

- spotting race conditions inherent in interfaces 
    - using a mutex or other mechanism to protect shared data -> NOT necessarily
        protected from race conditions
    - need to ensure that the private data is protected

- deadlocks
    - what is it? 
        - 2 or more threads
        - each pair of threads needs to lock both of a pair of mutexes to
            perform an operation
        - each thread has one mutex and is waiting for the other one
        - neither thread can proceed because each is waiting for the other to
            release its mutex
    - always lock mutexes in the same order !

- std::lock
    - a function that can lock 2 or more threads at the same time without the
        risk of deadlock
    - only useful if you need to acquire 2 or more locks together

- lock a mutex that is already locked -> undefined behavior
- std::adopt_lock -> mutexes are already locked -> adopt ownership

- deadlocks don’t just occur with locks -> 2 or more threads -> each calling
    join() for the other thread -> also deadlock
- don’t wait for another thread if there’s a chance it’s waiting for you !!

- how to avoid deadlocks:
    - avoid nested locks
        - don’t acquire a lock if you already hold one
        - each thread -> only one lock
        - if you need multiple locks -> use std::lock()
    - avoid calling user-supplied code while holding a lock
        - user code could acquire a lock and it could end in a deadlock
        - sometimes may be unavoidable
    - acquire locks in a fixed order
        - if absolutely must lock 2 or more, and cannot lock together using
            std::lock() -> acquire them in the same order in every thread
    - use a lock hierarchy 
        - particular case of lock ordering
        - check that the convention is adherent to at runtime
        - divide application into layers and identify all mutexes that may be
            locked in any given layer
        - when the code tries to lock a mutex, it isn’t permitted to lock if it
            already holds a lock from lower layer
        - check at runtime by assigning layer numbers to each mutex -> keep
            record of which mutex is locked by which thread 

- std::unique_lock
    - provides a bit more flexibility than std::lock_guard
    - doesn't always own the mutex it is associated with
    - can lock() and unlock() as many times as needed
    - comes with a cost
        - information has to be stored
        - has to be updated
    - has a flag which indicates if it owns a mutex or not (owns_lock() method
        to check)
        - needs to know in order to call unlock() in the destructor when needed
    - moveable
    - not copyable

- useful case for unique_lock -> when passed outside of the function and needs
    to be locked inside:

```
std::unique_lock<std::mutex> getLock()
{
    std::mutex m;
    std::unique_lock<std::mutex> lk{ m };
    prepareData();

    return lk;    // compiler calls move constructor here
}

void doStuff()
{
    std::unique_lock<std::mutex> lk{ getLock() }; // work under the same mutex
    doSomething();
}
```

- std::defer_lock -> do not lock the mutex on construction
- std::adopt_lock -> adopt locked mutex on construction

- other uses:
    - using std::lock() and std::unique_lock in swap operations
    - transfer mutex ownership between scopes (move)


- locking at an appropriate granularity
    - fine-grained lock -> protects small a amount of data
    - coarse-grained lock -> protects large a amount of data

    - lock mutex only when accessing the shared data !!
    - try to do processing of that data outside of the lock, so that no other
        threads are blocked while not accessing data -> use std::unique_lock
    - locking at an appropriate granularity is not only about the amount of data
        locked, it’s about how long the lock is held and what operations are
        performed while the lock is held
    - in general, a lock should be held for only the minimum possible time
        needed to perform the required operations

- locking one mutex at a time in a comparison operator

```
bool operator==(Foo const& lhs, Foo const& rhs)
{
    if (&lhs == &rhs)
    {
        return true;
    }
    int const lhsValue = lhs.getValue(); // Data race can happen between this
                                         // and the next line. Beware !!
    int const rhsValue = rhs.getValue();
    return lhsValue == rhsValue;
}
```

- if you don’t hold the required locks for the entire duration of an operation,
    you’re exposing yourself to race conditions
- sometimes, there isn’t an appropriate level of granularity to because not all
    access of the data structure require the same level of protection. In this
    case, it might be appropriate to use an alternative mechanism, instead of a
    plain std::mutex

- protecting shared data during initialization

    - lazy initialization - common single-threaded code -> each operation that
        requires the resource first checks to see if it has been initialized and
        then initializes it before use if not

```
std::shared_ptr<someResource> resourcePtr;

void foo()
{
    if (!resourcePtr)
    {
        resourcePtr.reset(new someResource);
    }
    resourcePtr.doSomething();
}
```

- the only part that needs protection is when used in multithreaded application,
    during the initialization !

```
std::shared_ptr<someResource> resourcePtr;
std::mutex resourceMutex;

void foo()
{
    std::unique_lock<std::mutex> lk(resourceMutex);
    if (!resourcePtr)
    {
        resourcePtr.reset(new someResource);
    }
    lk.unlock();
    resourcePtr.doSomething();
}
```

- double-Checked locking pattern ( potential data race !! )
    - pointer is first read before acquiring the lock
    - lock is acquired only if the pointer is NULL
    - the pointer is checked again

- c++ Standard Library also provides a solution for this
    - std::once_flat
    - std::call_once
```
std::shared_ptr<someResource> resourcePtr;
std::once_flat resourceFlag;

void initResource()
{
    resourcePtr.reset(new someResource);
}

void foo()
{
    std::call_once(resourceFlag, initResource);
    resourcePtr->doSomething();
}
```

- static variable initialization
    - before c++11 -> race condition
    - since c++11 -> initialization guaranteed to be thread safe

class MyClass;
MyClass& getMyClassInstance()
{
    static MyClass instance; // initialization guaranteed to be thread-safe
    return instance;
}

- protecting rarely updated data structures
    - reader-writer mutex -> 2 different kind of usage:
        - exclusive access by a single "writer" thread
        - shared, concurrent access by multiple "reader" threads
    - boost::shared_mutex 
    - boost::shared_lock<boost::shared_mutex>


- recursive locking
    - std::recursive_mutex
    - multiple locks can be acquired on a single instance from the same thread
    - must release all locks before it can be locked by another thread
    - std::lock_guard<> and std::unique_lock<> will handle this automatically if
        used correctly
    - NOT recommended to be used -> best if change of design
# Chapter 4: Synchronizing concurrent operations

- synchronize actions between separate threads
- one thread -> wait for another thread to complete its task in order to continue
- C++ Standard Library: condition variables and futures

## 4.1 Waiting for an event or other condition

- for a thread to wait for another thread to finish it can:
    - continuously check for a flag to see if it has been set (NOT good because
		of poorly used processing power and time)
    - wait a small period of time before checks by using
		std::this_thread::sleep_for() function
        - improvement to the above method
        - hard to get the sleep period right (too long -> thread sleeps for no
			reason -> delay -> could end in frame drops etc.)
    - use the facilities from C++ Standard Library (preferred) to wait for the
		event itself
        - condition variable
            - associated with some event or other condition, and one or more
				threads can wait for that condition to be satisfied
            - notify one or more threads when condition is satisfied (wake up
				threads(s) and allow them to continue processing)

```
bool flag;
std::mutex mtx;

void waitForFlag()
{
    std::unique_lock<std::mutex> lk{mtx};

    while (!flag)
    {
        lk.unlock();

		// wait 100 milliseconds between checks
        std::this_thread::sleep_for(std::chrono::milliseconds{100});

        lk.lock();
    }
}
```

### 4.1.1 Waiting for a condition with condition variables

- C++ Standard Library
    - std::condition_variable
    - std::condition_variable_any

- both implementations need to work with mutex to provide synchronization, the
	latter being able to work with anything that meets some minimal criteria for
	being mutex-like (`_any`)

- std::condition_variable_any has a potential for adding costs in terms of size,
	performance, OS resources
- std::condition_variable is preferred unless additional flexibility is required

```
std::mutex mtx;
std::queue<Foo> dataQueue;
std::condition_variable dataCondition;

void dataPreparationThread()
{
    while (moreDataToPrepare())
    {
        const Foo data = prepareData();
        std::lock_guard<std::mutex> lk { mtx };
        dataQueue.push(data);
        dataCondition.notify_one();
    }
}

void dataProcessingThread()
{
    while (true)
    {
        std::unique_lock<std::mutex> lk{mtx};

        dataCondition.wait( lk, []() {
            return !dataQueue.empty();
        });

        Foo data = dataQueue.front();
        dataQueue.pop();

        lk.unlock();

        process(data);
        if (isLastChunk(data))
            break;
    }
}
```

- wait() locks the unique_lock when checking the predicate (unique_lock must be
	used):
    - true -> continue the program
    - false -> unlock the unique_lock

- spurious wake - when the mutex is not acquired by a response from the tread
	it’s waiting for

### 4.1.2 Building a thread-safe queue using condition variables

- 3 groups of operations
    - state { empty(), size() }
    - query { front(), back() }
    - modify { push(), pop(), emplace() }

- front() + pop() -> a single function call (like top() and pop() for stack)
- when using a queue to pass data between threads, the receiving thread often
	needs to wait for the data -> pop:
    - try_pop() -> returns immediately (with an indication of failure) event it
		there wasn't a value to pop
        - bool try_pop(T& value)
        - std::shared_ptr<T> try_pop()
    - wait_and_pop() -> wait until there is a value to retrieve
    	- bool wait_and_pop(T& value)
        - std::shared_ptr<T> wait_and_pop()

- * *Read chapter for queue example* *

- if the waiting thread is only going to wait once (so when the condition is
	true, it will never have to wait for the condition variable again), a
	condition variable may not be the best choice of synchronization mechanism.
    - especially true if the condition being waiter for is the availability of a
	particular piece of data => future

## 4.2 Waiting for one-off events with futures

- C++ Standard Library's <future> library provides 2 types:
    - unique futures: std::future<>
    - shared futures: std::shared_future<>

### 4.2.1 Returning values from background tasks

--std::async()--
- used to start an ascync task for which you don't need the result right away
- returns a std::future, which will eventually hold the return value of the
	function
- get() - blocks the thread until the future is 'ready' and then returns the
	value

- parameters:
    - first: function address
    - second: object on which the function is called (if the case)
    - the rest: function parameters
- if the arguments are rvalues, the copies are created by MOVING the originals
    - this allows the use of move-only types as both the function object and
    	the arguments

- by default, it's up to the implementation whether std::async starts a new
thread or whether the task runs synchronously when the future is waited for
	- you can specify which to use with an additional parameter:
		- std::launch::deferred - function call to be deferred until get() or
			wait() called on the std::future
		- std::launch::async - function must be run on its own thread
	- if the function call is deferred, it may never actually run

### 4.2.2 Associating a task with a future

- std::packaged_task<> ties a future to a function or callable object
- when the std::packaged_task<> is invoked, it calls the associated
	function/callable object and makes the future ready, with the return value
	stored as associated data
- building block for thread pools or other task management schemes (running
	each task on its own thread or running them all sequentially on a
	particular background thread
- if a large operation can be divided into self-contained sub-tasks, each of
	these can be wrapped in a std::packaged_task<> instance, and then that
	instance can be passed to the task scheduler or thread pool. The scheduler
	just deals with std::packaged_tasks<> instead of individual functions
- when constructing a std::packaged_task<>, the function / callable object
	passed, its arguments and the return type must match!
	- return type represents the type of std::future<> returned from
		get_future()
	- argument list of the function signiture - arguments for the function call
		operator

e.g. partial class definition for specialization of std::packaged_task<>:

template<>
class packaged_task<std::string(std::vector<char>-, int)>
{
public:
	template<typename Callable>
	explicit packaged_task<Callable&& f)
	std::future<std::string> get_future()
	void operator()(std::vector<char>-, int)
}

### Passing tasks between threads
- many GUI frameworks require that updates for the GUI to be done from specific
	threads, so if another thread wants to update the GUI it must send a
	message to the right thread in order to do so
	- std::packaged_task<> provides a way of doing this without requiring a
		custom message for each and every GUI-related activity
	- e.g.: "gui_thread_packaged_task.cpp"

### 4.2.3 Making (std::) promises

- std::promise<T> sets a value (of type T) which can later be read through an
	associated std::future<T> object
	- std::promise and std::future pair:
		- the waiting thread could block on the future
		- the thread providing the data could use the std::promise half of the
			pairing to set the associated value and make the std::future ready
- std::promise<>
	- get_future() - obtain the associated std::future object
	- set_value() - set the value -> std::future becomes ready

- if std::promise is destroyed before setting a value -> exception is stored

### 4.2.4 Saving an exception for the future

- if the function call invoked as part of std::async throws an exception, that
	exception is stored in the std::future in place of a stored value, the
	future becomes ready, and a call to get() rethrows that stored exception
	- same with std::packaged_task<>
	- note: it may be a copy or the original exception object (based on impl)

- set_value()
- set_exception()

### 4.2.5 Waiting from multiple threads

- access std::future from multiple threads without synchronization -> data race
	and undefined behavior
- std::future -> unique ownership of the asynchronous result
	- only one thread can retrieve the value
	- after get() -> no value left to be retrieved
- std::future -> only moveable
	- has a method called shared() that creates a shared_future<>

- std::shared_future<>
	- copyable
	- unsynchronized -> lock or each thread should have a copy (preffered way)

## 4.3 Waiting with a time limit

- 2 sorts of timeouts:
	- `_for` -> duration-based timeout (30 milliseconds)
	- `_until` -> absolute timeout (17:30:35.04213 UTC, November 11, 2018)

### 4.3.1 Clocks

- source of time information:
	- the time NOW
	- the type of the value used to represent the times obtained from the clock
	- the tick period of the clock
	- wether or not the clock ticks at a uniform rate (steady clock)

### 4.3.2 Duration

... ???

### 4.3.3 Time Points

... ???

### 4.3.4 Functions that accept timeouts

- std::this_thread::sleep_for();
- std::this_thread::sleep_until();

- you can use timeouts when trying to acquire a mutex(if the mutex supports it)
    - do **not** support timeouts: std::mutex and std::recursive_mutex
    - do support timeouts: std::timed_mutex and std::recursive_timed_mutex
        - try_for_lock()
        - try_lock_until()

- check book(or official documentation) for which functions from c++ Standard
    Library accept timeouts, what are their parameters and return types

## 4.4 Using synchronization of operations to simplify code

- Using the synchronization facilities described so far in this chapter as
building blocks allows you to focus on the operations that need synchronizing
rather than the mechanics. One way this can help simplify your code is that it
accommodates a much more functional (in the sense of functional programming)
approach to programming concurrency. Rather than sharing data directly between
threads, each task can be provided with the data it needs, and the result can be
disseminated to any other threads that need it through the use of futures.


### 4.4.1 Functional programming with futures

- **functional programming (FP)**
    - style of programming
    - the result of a function call depends solely on the parameters!
    - doesn't depend on any external state
    - invoke a function twice with the same params -> exactly the same result
    - a **pure** function doesn't **modify** any external state!
    - the effects of the function are entirely limited to the return value!

- if this would be applied to concurrent code:
    - many of the problems associated with shared memory disappear
    - no possible race condition if no shared memory involved
    - no need to protect shared data with mutexes

- a future can be passed around between threads to allow the result of one
    computation to depend on the result of another, without any explicit access
    to shared data

- see `quicksort_sequential_and_parallel.cpp` for implementation

- it’s worth noting that it’s perfectly conforming for an implementation of
    std::async to start a new thread for each task (even in the face of massive
    oversubscription) unless std::launch::deferred is explicitly specified
    or to run all tasks synchronously unless std::launch::async is explicitly
    specified
- if you’re relying on the library for automatic scaling, you’re advised to
    check the documentation for your implementation to see what behavior it
    exhibits

### 4.4.2 Synchronizing operations with message passing

- **Communicating sequential processes (CSP)**
    - threads are conceptually entirely separate, with no shared data
    - use communication channels that allow messages to be passed between them

- if there’s no shared data, each thread can be reasoned about entirely
    independently, purely on the basis of how it behaves in response to the
    messages that it received
- each thread is therefore effectively a state machine: when it receives a
    message, it updates its state in some manner and maybe sends one or more
    messages to other threads, with the processing performed depending on
    the initial state
- writing such threads:
    1. formalize this and implement a **Finite State Machine model**
    2. the state machine can be **implicit in the structure of the application**

    - however you choose to implement each thread, the separation into
        independent processes has the potential to remove much of the
        complication from shared-data concurrency and therefore make programming
        easier, lowering the bug rate

- true communicating sequential processes:
    - no shared data
    - all communication passed through message queues
    - because c++ threads share an address space, it’s not possible to enforce
        this requirement
    - discipline:
        - application and library authors have the responsibility to ensure that
            don't share data between the threads
        - message queues must be shared in order for the threads to communicate,
            but the details can be wrapped in the library

- check `atm_implementation.cpp` (TO-DO: write and understand example)

- **actor model**
    - there are several discrete actors in the system (each running on a
        separate thread), which send messages to each other to perform the task
        at hand
    - no shared state except that directly passed via messages

# 5. The C++ memory model and operations on atomic types

- multithreading aware memory-model
- without the memory model to defined exactly how the fundamental building
	blocks work, none of the facilities covered so far could be relied on to
	work
- no need for a lower-level language than C++
	- enough flexibility within C++ to do whatever they need without the
	language getting in the way, allowing then to get "close to the machine"
	when the need arises
	- atomic types and operations allow just that, providing facilities for low
		level synchronization operations that will commonly reduce the one or
		two CPU instructions

## 5.1. Memory model basics

- 2 aspects of the memory model:
	- **structural** aspects -> how things are laid out in memory
	- **concurrency** aspects

###5.1.1. Objects and memory locations

- all data in C++ is made up of **objects**
	- it's a statement about the building blocks of data in C++
	- **object** -> "region of storage"
		- it goes to on to assign properties to these objects, such as their
			type and lifetime
	- they can be simple values of a fundamental type (int, float), or instances
		of user-defined classes
	- some objects (arrays, instances of derived classes, instances of classes
		with non-static data members) have subobjects, but others don't
	- whatever its type, an object is stored in one or more memory locations
		- each such memory location is either and object (or subobject) of a
			scalar type such as __unsigned short__ or __my_Class*__ or a
			sequence of adjacent bit fields
			- if you use bit fields, there is an important point to note:
				**though adjacent bit fields are distinct objects, they're still
				counted as the same memory location**!

- every variable is an object! including those that are members of other objects
- every object occupies **at least one** memory location
- variables of fundamental type (int, char) are exatly one memory location,
	whatever their size, even if they're adjacent or part of an array
- adjacent bit fields are part of the same memory location

### 5.1.2. Objects, memory location, and concurrency

- everything hinges on those memory locations!
	- it two threads access separate memory locations, everything's fine
	- if two threads access the same memory location -> potential issue:
		- if neither thread is updating the memory location, all good
			- read-only data doesn't need protection or synchronization
		- if either thread is modifying the data, there's a potential for race
			condition

- in order to avoid the race condition, there has to be an enforced ordering
	between the access in the two threads. Options:
	- mutexes -> if the mutex is locked prior to both accesses, only one thread
		can access the memory location at a time
	- synchronization properties of **atomic** operations, either on the same or
		other memory locations to enforce an ordering between the accesses in
		the two threads
		- if more than 2 threads access the same memory location, each pair of
			accesses must have a defined ordering

- if there is no enforced ordering between 2 accesses to a single memory
	location from separate threads (one or both of those accesses is NOT atomic,
	and one or both is a write) then this is a data race and causes undefined
	behavior

- **IMPORTANT**
	- Undefined behavior is one of the nastiest corners of C++
	- once an application contains any undefined behavior, all bets are off!
		- the behavior of the complete application is now undefined, and it may
			do nothing at all
	- data races should be avoided at all costs!


- data races can be avoided by using atomic operations to access the memory
	location involved in the race
	- this doesn't prevent the race itself (which of the atomic operations
		touches the memory location first is still not specified) but it does
		bring the program back into the realm of defined behavior

### 5.1.3. Modification orders

- every object in a C++ program has a defined **modification order** composed
	of all the writes to that object from all threads in the program, starting
	with the object's initialization
	- in most cases, this order will vary between the runs, but in any given
		execution of the program, all threads in the system must agree on the
		order
- if the object in question isn't atomic, you're responsible for making
	certain that there's sufficient synchronization to ensure that threads
	agree on the modification order of each variable
	- if different threads see distinct sequences of values for a single
		variable -> data race and undefined behavior
	- if using atomic operations -> the compiler is responsible for ensuring
		that the necessary synchronization is in place

- this requirement means that certain kinds of speculative execution aren't
	permitted, because once a thread has seen a particular entry in the
	modification order, subsequent reads from that thread must return later
	values, and subsequent writes from that thread to that object must occur
	later in the modification order!
	- also, a read of an object that follows a write to that object in the same
		thread must either return the value written or another value that occurs
		later in the modification order of that object
	- although all threads must agree on the modification orders of each
		individual object in a program, they don't necessarily have to agree on
		the relative order of operations on separate objects
- !!! re-read last idea !!!

## 5.2. Atomic operations and types in C++
