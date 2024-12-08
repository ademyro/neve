  × [E00X] type mismatch
    ╭─ src/main.neve:2:5 in add
  2 │   a + b
    ·       ┬
    ·       ╰─ expected Int, got Str 
    ·
    ├─ the Int implementation for Add.add is defined as
    │   fun add(other Int)
    ·           ─────────
    ·
    ╰─ 
  ✓ here's what you can do:
    · replace b of Str with an Int
    · make a another Str instead of an Int
    ·
    ├─ you can also learn more about [type conversion in Neve](link).
    ╰─ confused?  run `nevec --whats E00X`.

