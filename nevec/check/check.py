from nevec.ast.ast import *
from nevec.ast.visit import Visit

from nevec.check.errs import *

from nevec.err.err import Err

class Check(Visit[bool]):
    def __init__(self):
        self.had_err: bool = False

    def fail(self, err: Err) -> bool:
        self.had_err = True
        err.print()

        return True

    def visit_Parens(self, parens: Parens) -> bool: 
        return self.visit(parens.expr)

    def visit_UnOp(self, un_op: UnOp) -> bool:
        expr = un_op.expr

        if un_op.op == UnOp.Op.NEG:
            if expr.type.is_num():
                return False

            un_op.type.poison()

            return self.fail(TypeErr(
                "can only negate Int or Float values",
                un_op.loc,
                expr
            ))
            
        if un_op.op == UnOp.Op.NOT:
            if expr.type == Types.BOOL:
                return False

            return self.fail(TypeErr(
                "can only flip booleans",
                un_op.loc,
                expr
            ))

        raise TypeError(
            f"unary op {un_op.op.name()} not implemented in check.py"
        )
    
