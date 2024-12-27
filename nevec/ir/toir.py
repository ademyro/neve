from nevec.ast.ast import *
from nevec.ast.visit import Visit

from nevec.ir.ir import *

class ToIr(Visit[Ast, Ir]):
    def visit_Parens(self, parens: Parens) -> Ir: 
        return self.visit(parens.expr)

    def visit_UnOp(self, un_op: UnOp) -> IUnOp:
        expr = self.visit(un_op.expr)

        return IUnOp(
            IUnOp.Op(un_op.op.value),
            expr,
            un_op.loc,
            un_op.type
        )

    def visit_Bitwise(self, bitwise: Bitwise) -> IBinOp:
        left = self.visit(bitwise.left)
        right = self.visit(bitwise.right)

        op_lexeme = bitwise.tok.lexeme

        return IBinOp(
            left,
            IBinOp.Op(bitwise.op.value),
            right,
            op_lexeme,
            bitwise.loc,
            bitwise.type
        )

    def visit_Comparison(self, comparison: Comparison) -> IBinOp:
        left = self.visit(comparison.left)
        right = self.visit(comparison.right)

        op_lexeme = comparison.tok.lexeme

        return IBinOp(
            left,
            IBinOp.Op(comparison.op.value),
            right,
            op_lexeme,
            comparison.loc,
            comparison.type
        )

    def visit_Arith(self, arith: Arith) -> IBinOp:
        left = self.visit(arith.left)
        right = self.visit(arith.right)

        op_lexeme = arith.tok.lexeme

        return IBinOp(
            left,
            IBinOp.Op(arith.op.value),
            right,
            op_lexeme,
            arith.loc,
            arith.type
        )

    def visit_Int(self, i: Int) -> IInt:
        return IInt(
            i.value,
            i.loc,
            i.type
        )

    def visit_Float(self, f: Float) -> IFloat:
        return IFloat(
            f.value,
            f.loc,
            f.type
        )

    def visit_Bool(self, b: Bool) -> IBool:
        return IBool(
            b.value,
            b.loc,
        )

    def visit_Str(self, s: Str) -> IStr:
        return IStr(
            s.value,
            s.loc,
            s.type
        )

    def visit_Interpol(self, interpol: Interpol) -> IInterpol:
        expr = self.visit(interpol.expr)
        next = self.visit(interpol.next)

        return IInterpol(
            interpol.left,
            expr,
            next,
            interpol.loc,
            interpol.type
        )

    def visit_Nil(self, nil: Nil) -> INil:
        return INil(
            nil.loc
        )
