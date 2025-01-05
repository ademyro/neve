# Neve
Neve is a modern, hybrid-paradigm programming language designed to deliver reliability through static typing, while providing users a soothing* syntax to work with.

## Example
```rb
idea Nat = Int | self >= 0

fun fib(n Nat)
  match n
    | < 2 = n
    | else = fib(n - 1) + fib(n - 2)
  end
end

fun main
  let fib_seq = 0..10 | fib n

  puts fib_seq
end
```

\* Syntactical aesthetics are ultimately subjective, and Neve's syntax may not be considered 'soothing' by everyone.

## So, what *happened*?

The project has been moved to [its own organization!](https://github.com/neve-lang)  The compiler and the interpreter VM have been separated into two different projects.

This repository will no longer receive updates--updates will take place in the new repositories instead.
