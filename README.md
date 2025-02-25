# Scheme  
I have implemented an interpreter for a LISP-like programming language, specifically a subset of Scheme.  

The language consists of:  
- **Primitive types**: integers, booleans, and _symbols_ (identifiers).  
- **Composite types**: pairs and lists.  
- **Variables** with lexical scoping.  

The program evaluates expressions in the language and returns the result of their execution.  

```
    1 => 1  
    (+ 1 2) => 3  
```  
The `=>` notation in the examples above and below separates the expression from the result of its evaluation.  

## Expression Evaluation  
The execution of the language occurs in 3 stages:  

**Tokenization** - Converts the program text into a sequence of atomic tokens.  

**Parsing** - Converts the sequence of tokens into an [Abstract Syntax Tree (AST)](https://en.wikipedia.org/wiki/Abstract_syntax_tree).  

**Evaluation** - Recursively traverses the program's AST and transforms it according to a set of rules.  

### Example  

The expression  
```
    (+ 2 (/ -3 +4))  
```  
after tokenization, becomes a list of tokens:  
```
    {  
        OpenParen(),  
        Symbol("+"),  
        Number(2),  
        OpenParen(),  
        Symbol("/"),  
        Number(-3),  
        Number(4),  
        CloseParen(),  
        CloseParen()  
    }  
```  

The sequence of tokens, after parsing, is transformed into a tree:  
```
    Cell{  
        Symbol("+"),  
        Cell{  
            Number(2),  
            Cell{  
                Cell{  
                    Symbol("/"),  
                    Cell{  
                        Number(-3),  
                        Cell{  
                            Number(4),  
                            nullptr  
                        }  
                    }  
                },  
                nullptr  
            }  
        }  
    }  
```  

The result of evaluating the expression will be:  
```
    (+ 2 (/ -3 +4)) => 1  
```
