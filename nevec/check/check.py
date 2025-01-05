from nevec.ast.ast import *
from nevec.ast.visit import Visit

from nevec.check.errs import *

from nevec.err.err import Err

class Check(Visit[Ast, bool]):
    def __init__(self):
        self.had_err: bool = False

    def fail(self, err: Err) -> bool:
        self.had_err = True
        err.print()

        return True

    def visit_Program(self, program: Program) -> bool:
        return self.visit(program.expr)

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

    def visit_Concat(self, concat: Concat) -> bool:
        if self.visit(concat.left):
            return True
    
        if self.visit(concat.right):
            return True

        if concat.type.is_ignorable():
            return True

        if concat.type == Types.UNKNOWN:
            left = concat.left
            right = concat.right

            # right is always the culprit, as for the parser to produce a 
            # Concat node, it must find a Str node on the left hand side
            culprit = right

            utf_conversion = (
                ".utf" + culprit.type.name[:-2]
                if left.type != Types.STR
                else ""
            )

            loc_to_replace = (
                Loc.right_after(culprit.loc)
                if not isinstance(culprit, Op)
                else culprit.loc
            )

            show_fix = (
                ".show" if not culprit.type.is_str() else ""
            ) + utf_conversion

            fix = (
                show_fix
                if not isinstance(culprit, Op)
                else f"({culprit})" + show_fix
            )

            if not culprit.type.is_str():
                return self.fail(TypeErr(
                    f"cannot concatenate {culprit.type} to a Str type",
                    concat.loc,
                    left,
                    right
                ).suggest(
                    Suggestion(
                        f"you can convert '{culprit}' to a Str",
                        f"converts it to {left.type} (not implemented yet)",
                        loc_to_replace,
                        fix,
                        insert=not isinstance(culprit, Op)
                    )
                ))
            else:
                # otherwise, culprit is a Str, but doesn't have the right 
                # encoding
                return self.fail(TypeErr(
                    f"cannot concatenate {culprit.type} to {left.type}",
                    concat.loc,
                    left,
                    right
                ).suggest(
                    Suggestion(
                        f"you can convert '{culprit}' to a {left.type}",
                        f"converts it to {left.type} (not implemented yet)",
                        loc_to_replace,
                        fix,
                        insert=not isinstance(culprit, Op)
                    )
                ))

        return False

    def visit_Int(self, i: Int) -> bool:
        _ = i
        
        return False

    def visit_Float(self, f: Float) -> bool:
        _ = f

        return False

    def visit_Bool(self, b: Bool) -> bool:
        _ = b

        return False

    def visit_Str(self, s: Str) -> bool:
        _ = s

        return False

    def visit_Interpol(self, interpol: Interpol) -> bool:
        _ = interpol

        # TODO: check if each expression implements Show
        return False

    def visit_Nil(self, nil: Nil) -> bool:
        _ = nil

        return False

    def visit_Ast(self, ast: Ast) -> bool:
        _ = ast

        return True
