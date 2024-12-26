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
        if self.visit(un_op.expr):
            return True

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
        had_err = False

        if self.visit(bitwise.left):
            return True

        if self.visit(bitwise.right):
            return True 

        if bitwise.type.is_ignorable():
            return True

        if bitwise.type == Types.UNKNOWN:
            if bitwise.left.type != bitwise.right.type:
                had_err = self.fail(TypeErr(
                    "operand types don't match",
                    bitwise.loc,
                    bitwise.left,
                    bitwise.right
                ))
            
            if (
                bitwise.left.type != Types.INT or
                bitwise.right.type != Types.INT
            ):
                had_err = self.fail(TypeErr(
                    "operands of bitwise operation must be Int",
                    bitwise.loc,
                    bitwise.left,
                    bitwise.right
                ).add(
                    Note(
                        NoteType.HARMLESS,
                        bitwise.tok.loc,
                        "only accepts Int"
                    ),
                    on_line=bitwise.loc.line
                ))

        return had_err

    def visit_Comparison(self, comparison: Comparison):
        if self.visit(comparison.left):
            return True

        if self.visit(comparison.right):
            return True

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
        had_err = False

        if self.visit(arith.left):
            return True

        if self.visit(arith.right):
            return True

        if arith.type.is_ignorable():
            return True

        if arith.type == Types.UNKNOWN:
            if arith.left.type != arith.right.type:
                had_err = self.fail(TypeErr(
                    "operand types don't match",
                    arith.loc,
                    arith.left,
                    arith.right
                ))

            if (
                not arith.left.type.is_num() or
                not arith.right.type.is_num()
            ):
                had_err = self.fail(TypeErr(
                    "operands of arithmetic operation must be "
                    "either Int or Float",
                    arith.loc,
                    arith.left,
                    arith.right
                ).add(
                    Note(
                        NoteType.HARMLESS,
                        arith.tok.loc,
                        "only accepts Int or Float—assuming the types match"
                    ),
                    on_line=arith.loc.line
                ))

        return had_err

    def visit_Int(self, i: Int):
        _ = i
        
        return False

    def visit_Float(self, f: Float):
        _ = f

        return False

    def visit_Bool(self, b: Bool):
        _ = b

        return False

    def visit_Str(self, s: Str):
        _ = s

        return False

    def visit_Interpol(self, interpol: Interpol):
        _ = interpol

        # TODO: check if each expression implements Show
        return False

    def visit_Nil(self, nil: Nil):
        _ = nil

        return False
