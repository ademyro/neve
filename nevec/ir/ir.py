from typing import List

from enum import auto, Enum

from nevec.ast.ast import *
from nevec.ast.type import Type, Types

from nevec.ir.sym import *

from nevec.opcode.opcode import Opcode
from nevec.opcode.const import Const

from nevec.lex.tok import Loc

class IOp:
    def __init__(self, sym: Sym, loc: Loc):
        self.sym: Sym = sym
        self.loc: Loc = loc


class IRet(IOp):
    def __repr__(self) -> str:
        return f"ret {self.sym}"


class IExpr:
    def __init__(self, type: Type, loc: Loc):
        self.type: Type = type
        self.loc: Loc = loc


class TAC:
    def __init__(
        self,
        sym: Sym,
        expr: IExpr | Sym | IOp,
        loc: Loc,
        ops: List["TAC"]=[]
    ):
        self.sym: Sym = sym
        self.expr: IExpr | Sym | IOp = expr
        self.loc: Loc = loc

        # we can be 100% sure that this moment is the next moment
        # thanks to SSA
        self.moment: Moment = self.sym.first

        self.ops: List[TAC] = (
            ops 
            if ops != [] or isinstance(self.sym, NamelessSym) 
            else [self]
        )

    def next_moment(self) -> Moment:
        return self.moment + 1
    
    def __add__(self, other: "TAC") -> "TAC":
        return TAC(
            self.sym,
            self.expr,
            self.loc,
            
            other.ops + self.ops
        )

    def __repr__(self) -> str:
        if isinstance(self.expr, IOp | NamelessSym):
            return str(self.expr)

        return f"{self.sym.full_name} = {self.expr}" 


class IUnOp(IExpr):
    class Op(Enum):
        NEG = auto() 
        NOT = auto()
        IS_NIL = auto()
        IS_NOT_NIL = auto()
        IS_ZERO = auto()

        def opcode(self) -> Opcode:
            return Opcode(Opcode.NEG.value + self.value - 1)


    def __init__(
        self,
        op: Op,
        operand: Sym,
        loc: Loc,
        type: Type,
    ):
        self.op: IUnOp.Op = op
        self.operand: Sym = operand

        self.loc: Loc = loc
        self.type: Type = type

    def __repr__(self) -> str:
        match self.op:
            case IUnOp.Op.NEG:
                return f"neg {self.operand}"

            case IUnOp.Op.NOT:
                return f"not {self.operand}"

            case IUnOp.Op.IS_NIL:
                return f"isnil {self.operand}"

            case IUnOp.Op.IS_NOT_NIL:
                return f"isnotnil {self.operand}"
            
            case IUnOp.Op.IS_ZERO:
                return f"isz {self.operand}"


class IBinOp(IExpr):
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

        def opcode(self) -> Opcode:
            return Opcode(Opcode.ADD.value + self.value - 1)


    def __init__(
        self,
        left: Sym,
        op: Op,
        right: Sym,
        op_lexeme: str,
        loc: Loc,
        type: Type,
    ):
        self.left: Sym = left
        self.op: IBinOp.Op = op
        self.right: Sym = right
        self.op_lexeme: str = op_lexeme

        self.loc: Loc = loc
        self.type = type

    def __repr__(self) -> str:
        if self.op_lexeme == "":
            return f"{self.left} {self.right}"

        return f"{self.left} {self.op_lexeme} {self.right}"


class IInt(IExpr):
    def __init__(
        self,
        value: int,
        loc: Loc,
        type: Type,
    ):
        self.value: int = value

        self.loc: Loc = loc
        self.type: Type = type


    def __repr__(self) -> str:
        return f"{self.value}"


class IFloat(IExpr):
    def __init__(
        self,
        value: float,
        loc: Loc,
        type: Type,
    ):
        self.value: float = value

        self.loc: Loc = loc
        self.type: Type = type


    def __repr__(self) -> str:
        return f"{self.value}"


class IBool(IExpr):
    def __init__(
        self,
        value: bool,
        loc: Loc,
    ):
        self.value: bool = value

        self.loc: Loc = loc
        self.type: Type = Types.BOOL


    def __repr__(self) -> str:
        return str(self.value).lower()


class IStr(IExpr):
    def __init__(
        self,
        value: str,
        loc: Loc,
        type: Type,
    ):
        self.value: str = value

        self.loc: Loc = loc
        self.type: Type = type


    def __repr__(self) -> str:
        return f"\"{self.value}\""


class INil(IExpr):
    def __init__(
        self,
        loc: Loc,
    ):
        self.loc: Loc = loc
        self.type: Type = Types.NIL

    def __repr__(self) -> str:
        return "nil"

