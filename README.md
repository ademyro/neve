# Neve
Neve is a modern, hybrid-paradigm programming language designed to deliver reliability through static typing, whiel providing users a soothing* syntax to work with.

## Example
```rb
class LinkedNode
  val Int
  next LinkedNode?

  fun LinkedNode.new(val Int)
    return LinkedNode with
      val = val
      next = nil
    end
  end
end

fun main
  let first_node = LinkedNode.new -1
  var current_node = first_node

  10.times |i| do
    current_node.next = LinkedNode.new i
    current_node = current_node.next
  end
end
```

* Syntactical aesthetics are ultimately subjective, and Neve's syntax may not be considered 'soothing' by everyone.
