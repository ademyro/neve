from nevec.ast.ast import *
from nevec.ast.visit import Visit

from nevec.ir.ir import *
from nevec.ir.reg import *

class ToIr(Visit[Ast, Ir]):
    def __init__(self):
        self.reg_manager: RegManager = RegManager()

    def visit_Parens(self, parens: Parens) -> Ir: 
        return self.visit(parens.expr)

    def visit_UnOp(self, un_op: UnOp) -> IUnOp:
        expr = self.visit(un_op.expr).must_be_expr()

        return IUnOp(
            IUnOp.Op(un_op.op.value),
            expr,
            un_op.loc,
            un_op.type,
            self.reg_manager.next()
        )

    def visit_Bitwise(self, bitwise: Bitwise) -> IBinOp:
        dependent = Dependent()

        left = self.visit(bitwise.left).must_be_expr()
        dependent.depends_on(left)

        right = self.visit(bitwise.right).must_be_expr()
        dependent.depends_on(right)

        op_lexeme = bitwise.tok.lexeme

        dependent.loosen_dependencies()

        return IBinOp(
            left,
            IBinOp.Op(bitwise.op.value),
            right,
            op_lexeme,
            bitwise.loc,
            bitwise.type,
            self.reg_manager.next()
        )

    def visit_Comparison(self, comparison: Comparison) -> IBinOp:
        dependent = Dependent()

        left = self.visit(comparison.left).must_be_expr()
        dependent.depends_on(left)

        right = self.visit(comparison.right).must_be_expr()
        dependent.depends_on(right)

        op_lexeme = comparison.tok.lexeme

        dependent.loosen_dependencies()

        return IBinOp(
            left,
            IBinOp.Op(comparison.op.value),
            right,
            op_lexeme,
            comparison.loc,
            comparison.type,
            self.reg_manager.next()
        )

    def visit_Arith(self, arith: Arith) -> IBinOp:
        dependent = Dependent()

        left = self.visit(arith.left).must_be_expr()
        dependent.depends_on(left)

        right = self.visit(arith.right).must_be_expr()
        dependent.depends_on(right)

        op_lexeme = arith.tok.lexeme

        dependent.loosen_dependencies()

        return IBinOp(
            left,
            IBinOp.Op(arith.op.value),
            right,
            op_lexeme,
            arith.loc,
            arith.type,
            self.reg_manager.next()
        )

    def visit_Int(self, i: Int) -> IInt:
        return IInt(
            i.value,
            i.loc,
            i.type,
            self.reg_manager.next()
        )

    def visit_Float(self, f: Float) -> IFloat:
        return IFloat(
            f.value,
            f.loc,
            f.type,
            self.reg_manager.next()
        )

    def visit_Bool(self, b: Bool) -> IBool:
        return IBool(
            b.value,
            b.loc,
            self.reg_manager.next()
        )

    def visit_Str(self, s: Str) -> IStr:
        return IStr(
            s.value,
            s.loc,
            s.type,
            self.reg_manager.next()
        )

    def visit_Interpol(self, interpol: Interpol) -> IInterpol:
        dependent = Dependent()

        expr = self.visit(interpol.expr).must_be_expr()
        dependent.depends_on(expr)

        next = self.visit(interpol.next).must_be_expr()
        dependent.depends_on(next)

        dependent.loosen_dependencies()

        return IInterpol(
            interpol.left,
            expr,
            next,
            interpol.loc,
            interpol.type,
            self.reg_manager.next()
        )

    def visit_Nil(self, nil: Nil) -> INil:
        return INil(
            nil.loc,
            self.reg_manager.next()
        )
