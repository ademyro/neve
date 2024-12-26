from enum import auto, Enum
from typing import Self

from nevec.ast.ast import *
from nevec.ast.type import Type, Types

from nevec.lex.tok import Loc

class Ir:
    def __init__(self, type: Type, loc: Loc):
        self.type: Type = type
        self.loc: Loc = loc


class IExpr(Ir):
    ...


class IUnOp(Ir):
    class Op(Enum):
        NEG = auto() 
        NOT = auto()
        IS_NIL = auto()
        IS_NOT_NIL = auto()
        IS_ZERO = auto()


    def __init__(self, op: Op, operand: Ir, loc: Loc, type: Type):
        self.op: IUnOp.Op = op
        self.operand: Ir = operand
        self.loc: Loc = loc
        self.type: Type = type

    def __repr__(self) -> str:
        match self.op:
            case IUnOp.Op.NEG:
                return f"-{self.operand}"

            case IUnOp.Op.NOT:
                return f"not {self.operand}"

            case IUnOp.Op.IS_NIL:
                return f"not {self.operand}?"

            case IUnOp.Op.IS_NOT_NIL:
                return f"{self.operand}?"
            
            case IUnOp.Op.IS_ZERO:
                return f"{self.operand} == 0"


class IBinOp(Ir):
    class Op(Enum):
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
        GT = auto()
        GTE = auto()
        LT = auto()
        LTE = auto()

        CONCAT = auto()

    def __init__(
        self,
        left: Ir,
        op: Op,
        right: Ir,
        op_lexeme: str,
        loc: Loc,
        type: Type
    ):
        self.left: Ir = left
        self.op: IBinOp.Op = op
        self.right: Ir = right
        self.op_lexeme: str = op_lexeme

        self.loc: Loc = loc
        self.type = type

    def __repr__(self) -> str:
        if self.op_lexeme == "":
            return f"{self.left} {self.right}"

        return f"{self.left} {self.op_lexeme} {self.right}"

class IInt(Ir):
    def __init__(self, value: int, loc: Loc, type: Type):
        self.value: int = value

        self.loc: Loc = loc
        self.type: Type = type

    def __repr__(self) -> str:
        return f"{self.value} as {self.type}"

class IFloat(Ir):
    def __init__(self, value: float, loc: Loc, type: Type):
        self.value: float = value

        self.loc: Loc = loc
        self.type: Type = type

    def __repr__(self) -> str:
        return f"{self.value} as {self.type}"


class IBool(Ir):
    def __init__(self, value: bool, loc: Loc):
        self.value: bool = value

        self.loc: Loc = loc
        self.type: Type = Types.BOOL

    def __repr__(self) -> str:
        return str(self.value).lower()


class IStr(Ir):
    def __init__(self, value: str, loc: Loc, type: Type):
        self.value: str = value

        self.loc: Loc = loc
        self.type: Type = type

    def __repr__(self) -> str:
        return f"\"{self.value}\" as {self.type}"


class IInterpol(Ir):
    def __init__(
        self, 
        left: str, 
        expr: Ir, 
        next: Ir, # Self | IStr 
        loc: Loc,
        type: Type
    ):
        self.left: str = left
        self.expr: Ir = expr
        self.next: Ir = next

        self.loc: Loc = loc
        self.type: Type = type

    def __repr__(self) -> str:
        return "".join(
            [
                "\"", 
                self.left, 
                "#{", 
                str(self.expr), 
                "}", 
                Str.trim_quotes(str(self.next)) 
            ]
        )


class INil(Ir):
    def __init__(self, loc: Loc):
        self.loc: Loc = loc
        self.type: Type = Types.NIL

    def __repr__(self) -> str:
        return "nil"
