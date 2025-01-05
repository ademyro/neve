from enum import auto, Enum

from nevec.opcode.emit import Emit

class Opcode(Enum):
    CONST = auto()
    CONST_LONG = auto()

    TRUE = auto()
    FALSE = auto()
    NIL = auto()

    ZERO = auto()
    ONE = auto()
    MINUS_ONE = auto()

    NEG = auto()
    NOT = auto()
    IS_NIL = auto()
    IS_NOT_NIL = auto()
    IS_ZERO = auto()
    SHOW = auto()

    ADD = auto()
    SUB = auto()
    MUL = auto()
    DIV = auto()
    SHL = auto()
    SHR = auto()
    BIT_AND = auto()
    BIT_XOR = auto()
    BIT_OR = auto()
    NEQ = auto()
    EQ = auto()
    GREATER = auto()
    LESS = auto()
    GREATER_EQ = auto()
    LESS_EQ = auto()

    CONCAT = auto()

    RET = auto()
    
    def raw(self) -> int:
        return self.value - 1

    def emit(self) -> bytes:
        return Emit.integer(self.raw(), size=1)[0]
