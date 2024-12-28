from typing import List

from enum import auto, Enum

from nevec.ast.ast import *
from nevec.ast.type import Type, Types

from nevec.compile.opcode import Opcode

from nevec.lex.tok import Loc

from nevec.ir.reg import Reg

class Ir:
    def __init__(self, type: Type, loc: Loc):
        self.type: Type = type
        self.loc: Loc = loc
    
    def must_be_expr(self) -> "IExpr":
        if not isinstance(self, IExpr):
            raise ValueError("malformed IR")
        
        return self


class IExpr(Ir):
    def __init__(self, type: Type, loc: Loc, reg: Reg, dependent: "Dependent"):
        self.type: Type = type
        self.loc: Loc = loc
        self.reg: Reg = reg
        self.dependent: Dependent = dependent

    def freeze_reg(self):
        self.reg = self.reg.copy()


class Dependent:
    def __init__(self):
        self.dependencies: List[IExpr] = []

    def depends_on(self, what: IExpr):
        if what.reg.state == Reg.State.TEMP:
            what.reg.state = Reg.State.NECESSARY

        self.dependencies.append(what)

    def loosen_dependencies(self):
        if self.dependencies == []:
            return

        node = self.dependencies.pop()

        # node.dependent.loosen_dependencies()
        node.reg.state = Reg.State.TEMP
        node.freeze_reg()

        self.loosen_dependencies()


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
        operand: Ir,
        loc: Loc,
        type: Type,
        reg: Reg,
        dependent: Dependent=Dependent()
    ):
        self.op: IUnOp.Op = op
        self.operand: Ir = operand

        self.loc: Loc = loc
        self.type: Type = type
        self.reg: Reg = reg
        self.dependent: Dependent = dependent

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
        left: Ir,
        op: Op,
        right: Ir,
        op_lexeme: str,
        loc: Loc,
        type: Type,
        reg: Reg,
        dependent: Dependent=Dependent()
    ):
        self.left: Ir = left
        self.op: IBinOp.Op = op
        self.right: Ir = right
        self.op_lexeme: str = op_lexeme

        self.loc: Loc = loc
        self.type = type
        self.reg: Reg = reg
        self.dependent: Dependent = dependent

    def __repr__(self) -> str:
        if self.op_lexeme == "":
            return f"({self.left} {self.right})"

        return f"({self.left} {self.op_lexeme} {self.right})"


class IInt(IExpr):
    def __init__(
        self,
        value: int,
        loc: Loc,
        type: Type,
        reg: Reg,
        dependent: Dependent=Dependent()
    ):
        self.value: int = value

        self.loc: Loc = loc
        self.type: Type = type
        self.reg: Reg = reg

        self.dependent: Dependent = dependent

    def __repr__(self) -> str:
        return f"{self.value} as {self.type}"


class IFloat(IExpr):
    def __init__(
        self,
        value: float,
        loc: Loc,
        type: Type,
        reg: Reg,
        dependent: Dependent=Dependent()
    ):
        self.value: float = value

        self.loc: Loc = loc
        self.type: Type = type
        self.reg: Reg = reg

        self.dependent: Dependent = dependent

    def __repr__(self) -> str:
        return f"{self.value} as {self.type}"


class IBool(IExpr):
    def __init__(
        self,
        value: bool,
        loc: Loc,
        reg: Reg,
        dependent: Dependent=Dependent()
    ):
        self.value: bool = value

        self.loc: Loc = loc
        self.type: Type = Types.BOOL
        self.reg: Reg = reg

        self.dependent: Dependent = dependent

    def __repr__(self) -> str:
        return str(self.value).lower()


class IStr(IExpr):
    def __init__(
        self,
        value: str,
        loc: Loc,
        type: Type,
        reg: Reg,
        dependent: Dependent=Dependent()
    ):
        self.value: str = value

        self.loc: Loc = loc
        self.type: Type = type
        self.reg: Reg = reg

        self.dependent: Dependent = dependent

    def __repr__(self) -> str:
        return f"\"{self.value}\" as {self.type}"


class IInterpol(IExpr):
    def __init__(
        self, 
        left: str, 
        expr: Ir, 
        next: Ir, # Self | IStr 
        loc: Loc,
        type: Type,
        reg: Reg,
        dependent: Dependent=Dependent()
    ):
        self.left: str = left
        self.expr: Ir = expr
        self.next: Ir = next

        self.loc: Loc = loc
        self.type: Type = type
        self.reg: Reg = reg

        self.dependent: Dependent = dependent

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


class INil(IExpr):
    def __init__(
        self,
        loc: Loc,
        reg: Reg,
        dependent: Dependent=Dependent()
    ):
        self.loc: Loc = loc
        self.type: Type = Types.NIL
        self.reg: Reg = reg
        self.dependent: Dependent = dependent

    def __repr__(self) -> str:
        return "nil"

