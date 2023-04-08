
* The default constructor generated by the compiler will call the default
    constructors from the data members, but it will NOT initialize the data
    members;
    e.g. A reference data member will NOT be initializer in the default
        constructor;

* All references need to be initialized!
    - a reference cannot be reasigned;
    - a reference can only be assigned at initialization;


* Dissalow functions:
    * f() = delete; // to remove default constructed functions;
    * Declare the function private and do not define it;
    * private destructor is good when objects should NOT be stored on the stack;

* Exceptions in destructors
    * multiple exceptions pending at the same time -> crash;
    * DO NOT throw exceptions in destructors;
    * Destructors may contain code that throw exceptions, but they must be
        caught in the destructor;

* The only code that is guaranteed to be executed after exception is thrown are
    the destructors of objects on the stack;
* Resource management needs to be tied to the lifespan of suitable objects in
    order to gain automatic deallocation and reclamation;

* Global variables defined in separate files have a random order of initialization;

* Object Oriented code:
    * separate interface from implementation;
    * encapsulate complex/complicated implementation;
    * communicate with outside world through interfaces;
        * that's why getters and setters exist;
        * add additional checks etc;
    * too many getters and setters -> maybe bad design, why do we have to own
        the data if it is used so much outside?;

* co-variant return type:
    * lets an overwritten function to have a different return type as long as
        the return type is derived from the base class's function's return type;