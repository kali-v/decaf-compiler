# decaf-compiler

The decaf compiler is implemented in C++ and utilizes Flex and Bison for the frontend. The backend generates LLVM IR. Note that code generation is not finished yet and is missing several features from the grammar.

## Build & Usage

Build using: `make`

Usage:
```
Usage: decaf [option]
Options:
 -s            Print symbol table
 -t            Print Abstract Syntax Tree (AST)
 -p            Print LLVM IR code
 -r FILENAME   Run the program
```

## Example

`examples/example.decaf`
```java
class Main {
    int a;
    int b;
    int c;

    void foo() {
        int b;
        b = 4;
        a = b + 1;
    }

    int bar() {
        int a;
        a = 12 || 50 % 32;
        return 2 + a;
    }

    void main() {
        foo();
        print(a);

        c = bar() - 1;
        print(c);
        print(a + 1);
    }
}
```

Compile to `example` executable and run:
```
$ ./decaf -r example < examples/example.decaf
5
31
6
```

Print LLVM IR:
```llvm
$ ./decaf -p < examples/example.decaf
; ModuleID = 'module'
source_filename = "module"

@d_format = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@a = global i32 0
@b = global i32 0
@c = global i32 0
@b.1 = global i32 0
@a.2 = global i32 0

define void @foo() {
foo:
  store i32 4, ptr @b.1, align 4
  %0 = load i32, ptr @b.1, align 4
  %1 = add i32 %0, 1
  store i32 %1, ptr @a, align 4
  ret void
}

define i32 @bar() {
bar:
  store i32 30, ptr @a.2, align 4
  %0 = load i32, ptr @a.2, align 4
  %1 = add i32 2, %0
  ret i32 %1
}

define void @main() {
main:
  call void @foo()
  %0 = load i32, ptr @a, align 4
  call void @printf(ptr @d_format, i32 %0)
  %1 = call i32 @bar()
  %2 = sub i32 %1, 1
  store i32 %2, ptr @c, align 4
  %3 = load i32, ptr @c, align 4
  call void @printf(ptr @d_format, i32 %3)
  %4 = load i32, ptr @a, align 4
  %5 = add i32 %4, 1
  call void @printf(ptr @d_format, i32 %5)
  ret void
}

declare void @printf(ptr)
```

Print symbol table:
```
$ ./decaf -s < examples/example.decaf`
Main: <class_type>
  a: int
  b: int
  c: int
  foo: <func_type> () -> void
    b: int
  bar: <func_type> () -> int
    a: int
  main: <func_type> () -> void
```

Print AST:
```
$ ./decaf -t < examples/example.decaf
<program> --> <classdec>
  <classdec> --> CLASS ID <classbody>
    ID --> Main
    <classbody> --> {  <vardecs>  <methdecs>  }
      <vardec> --> <type> ID <vardec>
        <type> --> <simpletype>
          <simpletype> --> int
        ID --> a
    ...
```


## Grammar

The input grammar is as follows. In the code, it has been converted to LALR for practicality.

```
*Program* -> ClassDeclaration+

*ClassDeclaration* -> **class** **identifier** ClassBody

*ClassBody* -> { VarDeclaration∗ ConstructorDeclaration∗ MethodDeclaration∗ }

*VarDeclaration* -> Type **identifier**;

*Type* -> SimpleType

*SimpleType* -> **int** | **identifier**

*ConstructorDeclaration* -> **identifier** ( ParameterList ) Block

*MethodDeclaration* -> ResultType **identifier** ( ParameterList ) Block

*ResultType* -> Type | **void** 

*ParameterList* -> ε | Parameter < , Parameter> ∗

*Parameter* -> Type **identifier**

*Block* -> { LocalVarDeclaration∗ Statement∗ }

*LocalVarDeclaration* -> Type **identifier** ;

*Statement* -> ;
| Name = Expression ;
| Name ( Arglist ) ;
| **print** ( Expression ) ;
| ConditionalStatement
| **while** ( Expression ) Statement
| **return** OptionalExpression ;
| Block

*Name* -> **this**
| **identifier**
| Name . **identifier**
| Name [ Expression ]

*Arglist* -> ε
| Expression < , Expression >∗

*ConditionalStatement* -> **if** ( Expression ) Statement
| **if** ( Expression ) Statement else Statement

*OptionalExpression* -> ε | Expression

*Expression* -> Name
| **number**
| **null**
| Name ( ArgList )
| **read** ( )
| NewExpression
| UnaryOp Expression
| Expression RelationOp Expression
| Expression SumOp Expression
| Expression ProductOp Expression
| ( Expression )

*NewExpression* -> **new** **identifier** ( Arglist )
| **new** SimpleType < [ Expression ] >+ < [] >∗

*UnaryOp* -> + | − | !

*RelationOp* -> == | ! = | <= | >= | < | >

*SumOp* -> + | − | ||

*ProductOp* -> ∗ | / | % | &&
```