# Compiler
- https://www.youtube.com/watch?v=cxNlb2GTKIc&list=PLTd6ceoshpreZuklA7RBMubSmhE0OHWh_&index=1&ab_channel=ComputerScience
- source code (human readable program) -> compiler -> object code (executable machine code)
- new programming languages and their compilers are developed hand in hand
    - inner workings of the compiler depend on the language of the source code being compiled
- how the compiler is implemented depends on the architecture of the target machine

## what makes a good compiler?
- works correctly
- detect all static errors
- produces meaningful diagnostics
- generates optimal machine code
- compiles quickly
- easy to use
- modular
- documented and easy to maintain

## Stages of compilation
- frontend:
    - lexical analysis
    - syntax analysis
    - intermediate code generation (some compilers)
    - intermediate code optimization (some compilers)
- backend:
    - code generation and optimization

### Lexical analysis (tokenization)
- done by the **Lexical analyzer / Lexer / Scanner**
- identifies lexemes within the source code
- white spaces and comments are discarded
- generates stream of tokens
- each token passed to the parser on request
- creates a symbol table of 'names' used in the source code
- may also create a string table
- very limited error checking at this stage

### Syntax analysis
- done by the **Syntax analyzer / Parser**
- checks that the rules of the language have been followed correctly
- syntax can be defined using a context free grammar
- *Backus-Naur Form (BFS)* is a well known notation for communicating grammar
- grammar is applied programatically to build a *parse tree*
- *parse tree* is refined to become an *abstract syntax tree* (some compilers have this as final result of the frontend)
- *abstract syntax tree* is traversed several times during **semantic analysis**
- symbol table is frequently accessed and updated
- symbol table is often implemented as a hash table
- polish and reverse polish represent expressions without parantheses
- expressions can also be represented using trees or directed acyclic graphs

#### Semantic analysis
- undeclared variables
- multiple declarations within the same scope
- misuse of reserved identifiers
- attempting to access a variable that is out of scope
- type mismatches
- parameter type mismatch when calling functions or procedures
- function return type must match the specified return type
- arithmetic operators must operate on numeric types
- the condition in an 'if' statement must evaluate to true or false
- the exit condition of a loop must evaluate to true or false

### Intermediate code generation
- low level, linearized, representation of the source code
- may be generated while or after the *abstract syntax tree* is built
- allows for significant machine independent optimizations
- independent of the target machine
- **Three Address Code (TAC)** is implemented by many compilers
- **backpatching** generates forward jump instructions
- instructions are grouped into **basic blocks**
- various optimizations may be possible for each basic block
- peephole optimization looks at code across basic blocks
- optimizations may require several passes over the intermediate code

#### Three Address Code
- common intermediate representation of a program
- not machine specific
- each assignment can have only one operator on the right hand side and up to 3 addresses in total
```
x = (y + z) * w;

=>

t1 = y + z;
t2 = t1 * w;
x = t2;
```
- instructions may be implemented as objects or as records
- each name actually a pointer to an entry in the symbol table

#### Backpatching
- generates forward jump instructions

#### Basic blocks
- sequences of instructions that are always executed together
- provide oportunities for machine independent optimizations
- analyze and optimize flow control
- make it easier to allocate and assign registers to variables

#### Basic blocks optimization
- some programming styles lead to superfluous intermediate code
- systematic generation of intermediate code results in superfluous code
- better word -> improvement, not optimization
- the observable behavior of the program must not be changed
- the compiler must work quickly
- what may happen:
    - change the order of instructions to free up registers early
    - elimination of duplicate expressions
    - copy propagation and constant folding
    - elimination of dead code
    - algebraic transformations
    - flow of control between basic blocks

### Object code generation
- depends on machine architecture (RISC or CISC)
- instruction selection by simple substitution
- instruction selection and ordering by tree rewriting
- instruction selection based on processing cost
- register descriptors to allocate and assign registers
- peephole optimization


#### Processor architectures:
- **RICS** (reduced instruction set computer)
    - few instructions in the instructions set
    - 3 address instructions
    - more instructions needed for task
    - one clock cycle per instruction
    - few addressing modes
    - many CPU registers
- **CISC** (complex instruction set computer)
    - many instructions in the instructions set
    - 2 address instructions
    - fewer instructions needed for task
    - several clock cycles per instruction
    - many addressing modes
    - few CPU registers

#### Instruction selection by tree rewriting
- instruction set described by a set of tree rewriting rules
- each rule is a simple tree template with a replacement node
- each rule corresponds to a machine instruction
- subtrees in the input tree are replaced according to the rules

#### Peephole optimization
- code examined through a sliding window
- remove redundant identifiers
- constant evaluation
- remove unreachable sections of code
- identify common sub-expressions
- unfold loops
- eliminating procedures
