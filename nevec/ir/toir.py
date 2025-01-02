from typing import Tuple

from nevec.ast.ast import *
from nevec.ast.visit import Visit

from nevec.ir.ir import *
from nevec.ir.reg import *

class ToIr(Visit[Ast, TAC]):
    DIGITS = "1234567890"

    def __init__(self):
        self.reg_manager: RegManager = RegManager()
        self.syms: Dict[str, Sym] = {}

        self.last_moment: Moment = 0

    def new_sym(self, moment: Moment, name: str="t") -> Sym:
        name, index = self.next_available_name(name)

        sym = Sym(name, index, moment)

        self.syms[sym.full_name] = sym

        return sym

    def next_available_name(self, name: str, index: int=0) -> Tuple[str, int]:
        full_name = name + str(index)

        if full_name not in self.syms.keys():
            return name, index

        return self.next_available_name(name, index + 1)

    def visit_Program(self, program: Program) -> TAC:
        expr = self.visit(program.expr)

        ret = IRet(
            expr.sym,
            expr.loc
        )

        tac = TAC(
            ret.sym,
            ret,
            expr.loc
        )

        return tac + expr

    def visit_Parens(self, parens: Parens) -> TAC: 
        return self.visit(parens.expr)

    def visit_UnOp(self, un_op: UnOp) -> TAC:
        operand = self.visit(un_op.expr)
        
        expr = IUnOp(
            IUnOp.Op(un_op.op.value),
            operand.sym,

            un_op.loc,
            un_op.type
        )

        tac = TAC(
            self.new_sym(operand.next_moment()),
            expr,
            expr.loc
        )

        return tac + operand

    def visit_Bitwise(self, bitwise: Bitwise) -> TAC:
        left = self.visit(bitwise.left)
        right = self.visit(bitwise.right)

        op_lexeme = bitwise.tok.lexeme

        expr = IBinOp(
            left.sym,
            IBinOp.Op(bitwise.op.value),
            right.sym,
            op_lexeme,

            bitwise.loc,
            bitwise.type,
        )

        moment = right.next_moment()

        tac = TAC(
            self.new_sym(moment),
            expr,
            expr.loc
        )

        return tac + left + right

    def visit_Comparison(self, comparison: Comparison) -> TAC:
        left = self.visit(comparison.left)
        right = self.visit(comparison.right)

        op_lexeme = comparison.tok.lexeme

        expr = IBinOp(
            left.sym,
            IBinOp.Op(comparison.op.value),
            right.sym,
            op_lexeme,

            comparison.loc,
            comparison.type,
        )

        moment = right.next_moment()

        return TAC(
            self.new_sym(moment),
            expr,
            expr.loc
        ) + right + left

    def visit_Arith(self, arith: Arith) -> TAC:
        left = self.visit(arith.left)
        right = self.visit(arith.right)

        op_lexeme = arith.tok.lexeme

        expr = IBinOp(
            left.sym,
            IBinOp.Op(arith.op.value),
            right.sym,
            op_lexeme,

            arith.loc,
            arith.type,
        )

        moment = right.next_moment()

        return TAC(
            self.new_sym(moment),
            expr,
            expr.loc
        ) + right + left

    def visit_Int(self, i: Int) -> TAC:
        expr = IInt(
            i.value,
            i.loc,
            i.type,
        )

        self.last_moment += 1

        sym = NamelessSym(expr, self.last_moment - 1)
        return TAC(
            sym,
            sym,
            expr.loc
        )

    def visit_Float(self, f: Float) -> TAC:
        expr = IFloat(
            f.value,
            f.loc,
            f.type,
        )

        self.last_moment += 1

        sym = NamelessSym(expr, self.last_moment - 1)
        return TAC(
            sym,
            sym,
            expr.loc
        )

    def visit_Bool(self, b: Bool) -> TAC:
        expr = IFloat(
            b.value,
            b.loc,
            b.type,
        )

        self.last_moment += 1

        sym = NamelessSym(expr, self.last_moment - 1)
        return TAC(
            sym,
            sym,
            expr.loc
        )

    def visit_Str(self, s: Str) -> TAC:
        expr = IStr(
            s.value,
            s.loc,
            s.type,
        )

        self.last_moment += 1

        sym = NamelessSym(expr, self.last_moment - 1)
        return TAC(
            sym,
            sym,
            expr.loc
        )

    def visit_Nil(self, nil: Nil) -> TAC:
        expr = INil(
            nil.loc
        )

        self.last_moment += 1

        sym = NamelessSym(expr, self.last_moment - 1)
        return TAC(
            sym,
            sym,
            expr.loc
        )
