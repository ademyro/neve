# Neve
Neve is a modern, hybrid-paradigm programming language designed to deliver reliability through static typing, while providing users a soothing* syntax to work with.

## Example
```rb
fun main
  let my_tea = Tea with
    herbs = Herbs.Chamomile 
  end

  my_tea.brew
end
```

\* Syntactical aesthetics are ultimately subjective, and Neve's syntax may not be considered 'soothing' by everyone.

## So, what’s happening?

Neve’s compiler is going to be ported from C to Python.  This should allow us to implement complex optimizations more
efficiently.  We’re doing this because we want Neve to support separate compilation, and in doing so implement further 
optimizations on the Neve code before it’s translated to bytecode.

This means that we’re going to introduce two major changes:

* We’re getting rid of the REPL.  Having a separate compiler for Neve means longer compilation times, and a less snappy 
REPL experience.  Moreover, writing the bytecode output to a file and *then* having the Neve interpreter read it isn’t exactly
desirable--we don’t want to create a new file for the tiniest expressions.

* Neve will now have two components--`neve` and `nevec`.  `nevec` will compile the `.neve` file to a `.geada` bytecode 
file, and `neve` will interpret that file.  `nevec` will be implemented in Python, but we will immediately work on 
self-hosting it (reimplementing it in Neve) the moment we can.

Eventually, Neve will be moved to a GitHub organization instead of sitting on my GitHub account, and once that step 
is completed, `neve` and `nevec` will be hosted on two separate projects.
