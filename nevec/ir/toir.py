from typing import Tuple

from nevec.ast.ast import *
from nevec.ast.visit import Visit

from nevec.ir.ir import *
from nevec.ir.reg import *

class ToIr(Visit[Ast, TAC]):
    DIGITS = "1234567890"

    def __init__(self):
        self.syms: Dict[str, Sym] = {}

        self.ops: List[TAC] = []

    def next_moment(self) -> Moment:
        return len(self.ops)

    def new_sym(self, moment: Optional[Moment]=None, name: str="t") -> Sym:
        moment = moment if moment else self.next_moment()

        name, index = self.next_available_name(name)

        sym = Sym(name, index, moment)

        self.syms[sym.full_name] = sym

        return sym

    def next_available_name(self, name: str, index: int=0) -> Tuple[str, int]:
        full_name = name + str(index)

        if full_name not in self.syms.keys():
            return name, index

        return self.next_available_name(name, index + 1)

    def build_ir(self, ast: Ast) -> List[TAC]:
        if not isinstance(ast, Program):
            raise ValueError("Ast must begin with a Program node")

        self.visit(ast)

        return self.ops

    def visit_Program(self, program: Program) -> TAC:
        expr = self.visit(program.expr)

        ret = IRet(
            expr.sym,
            expr.loc
        )

        expr.sym.last_used(expr.next_moment())

        tac = TAC(
            ret.sym,
            ret,
            expr.loc
        )

        self.ops.append(tac)
        return tac

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
            self.new_sym(self.next_moment()),
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

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

        moment = self.next_moment()

        left.sym.last_used(moment)
        right.sym.last_used(moment)

        tac = TAC(
            self.new_sym(moment),
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

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

        moment = self.next_moment()

        left.sym.last_used(moment)
        right.sym.last_used(moment)

        tac = TAC(
            self.new_sym(moment),
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

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

        moment = self.next_moment()

        left.sym.last_used(moment)
        right.sym.last_used(moment)

        tac = TAC(
            self.new_sym(moment),
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Int(self, i: Int) -> TAC:
        expr = IInt(
            i.value,
            i.loc,
            i.type,
        )

        sym = self.new_sym()

        tac = TAC(
            sym,
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Float(self, f: Float) -> TAC:
        expr = IFloat(
            f.value,
            f.loc,
            f.type,
        )

        sym = self.new_sym()

        tac = TAC(
            sym,
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Bool(self, b: Bool) -> TAC:
        expr = IFloat(
            b.value,
            b.loc,
            b.type,
        )

        sym = self.new_sym()

        tac = TAC(
            sym,
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Str(self, s: Str) -> TAC:
        expr = IStr(
            s.value,
            s.loc,
            s.type,
        )

        sym = self.new_sym()

        tac = TAC(
            sym,
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Nil(self, nil: Nil) -> TAC:
        expr = INil(
            nil.loc
        )

        sym = self.new_sym()

        tac = TAC(
            sym,
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

