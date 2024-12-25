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
        
    def visit_Bitwise(self, bitwise: Bitwise) -> bool:
        if bitwise.type.is_ignorable():
            return True

        if bitwise.type == Types.UNKNOWN:
            if bitwise.left.type != bitwise.right.type:
                return self.fail(TypeErr(
                    "operand types don't match",
                    bitwise.loc,
                    bitwise.left,
                    bitwise.right
                ))
            
            # otherwise bitwise.left.type != bitwise.right.type
            return self.fail(TypeErr(
                "left and right operands must be integers",
                bitwise.loc,
                bitwise.left,
                bitwise.right
            ))

        return False

    def visit_Comparison(self, comparison: Comparison):
        if comparison.type.is_ignorable():
            return True

        if comparison.type == Types.UNKNOWN:
            return self.fail(TypeErr(
                "operand types don't match",
                comparison.loc,
                comparison.left,
                comparison.right
            ))

        return False

    def visit_Arith(self, arith: Arith):
        if arith.type.is_ignorable():
            return True

        if arith.type == Types.UNKNOWN:
            if arith.left.type != arith.right.type:
                return self.fail(TypeErr(
                    "operand types don't match",
                    arith.loc,
                    arith.left,
                    arith.right
                ))

            # otherwise, either arith.right or arith.left aren't nums
            return self.fail(TypeErr(
                "operands must be either Int or Float",
                arith.loc,
                arith.left,
                arith.right
            ))

        return False

    def visit_Int(self, i: Int):
        return False

    def visit_Float(self, f: Float):
        return False

    def visit_Bool(self, b: Bool):
        return False

    def visit_Str(self, s: Str):
        return False

    def visit_Interpol(self, interpol: Interpol):
        # TODO: check if each expression implements Show
        return False

    def visit_Nil(self, nil: Nil):
        return False
