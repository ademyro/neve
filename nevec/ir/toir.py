from typing import Tuple

from nevec.ast.ast import *
from nevec.ast.visit import Visit

from nevec.ir.ir import *
from nevec.ir.reg import *
from nevec.ir.val import Val

class ToIr(Visit[Ast, Tac]):
    DIGITS = "1234567890"

    def __init__(self):
        self.syms: Syms = Syms()

        self.ops: List[Tac] = []

    def new_sym[T](
        self,
        moment: Optional[Moment]=None,
        name: str="t",
        value: Optional[T]=None
    ) -> Sym:
        moment = moment if moment else self.next_moment()

        return self.syms.new_sym(moment, name, value)

    def next_moment(self) -> Moment:
        return len(self.ops)

    def build_ir(self, ast: Ast) -> List[Tac]:
        if not isinstance(ast, Program):
            raise ValueError("Ast must begin with a Program node")

        self.visit(ast)

        return self.ops

    def visit_Program(self, program: Program) -> Tac:
        expr = self.visit(program.expr)

        ret = IRet(
            expr.sym,
            expr.loc
        )

        expr.sym.last_used(expr.next_moment())

        tac = Tac(
            ret.sym,
            ret,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Parens(self, parens: Parens) -> Tac: 
        return self.visit(parens.expr)

    def visit_UnOp(self, un_op: UnOp) -> Tac:
        operand = self.visit(un_op.expr)
        
        expr = IUnOp(
            IUnOp.Op(un_op.op.value),
            operand,

            un_op.loc,
            un_op.type
        )

        moment = self.next_moment()
        operand.sym.last_used(moment)

        tac = Tac(
            self.new_sym(moment),
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Bitwise(self, bitwise: Bitwise) -> Tac:
        left = self.visit(bitwise.left)
        right = self.visit(bitwise.right)

        op_lexeme = bitwise.tok.lexeme

        expr = IBinOp(
            left,
            IBinOp.Op(bitwise.op.value),
            right,
            op_lexeme,

            bitwise.loc,
            bitwise.type,
        )

        moment = self.next_moment()

        left.sym.last_used(moment)
        right.sym.last_used(moment)

        tac = Tac(
            self.new_sym(moment),
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Comparison(self, comparison: Comparison) -> Tac:
        left = self.visit(comparison.left)
        right = self.visit(comparison.right)

        op_lexeme = comparison.tok.lexeme

        expr = IBinOp(
            left,
            IBinOp.Op(comparison.op.value),
            right,
            op_lexeme,

            comparison.loc,
            comparison.type,
        )

        moment = self.next_moment()

        left.sym.last_used(moment)
        right.sym.last_used(moment)

        tac = Tac(
            self.new_sym(moment),
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Arith(self, arith: Arith) -> Tac:
        left = self.visit(arith.left)
        right = self.visit(arith.right)

        op_lexeme = arith.tok.lexeme

        expr = IBinOp(
            left,
            IBinOp.Op(arith.op.value),
            right,
            op_lexeme,

            arith.loc,
            arith.type,
        )

        moment = self.next_moment()

        left.sym.last_used(moment)
        right.sym.last_used(moment)

        tac = Tac(
            self.new_sym(moment),
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Concat(self, concat: Concat) -> Tac:
        left = self.visit(concat.left)
        right = self.visit(concat.right)

        expr = IBinOp(
            left,
            IBinOp.Op(concat.op.value),
            right,

            "concat",

            concat.loc,
            concat.type,
        )

        moment = self.next_moment()

        left.sym.last_used(moment)
        right.sym.last_used(moment)

        tac = Tac(
            self.new_sym(moment),
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Show(self, show: Show) -> Tac:
        operand = self.visit(show.expr)

        expr = IUnOp(
            IUnOp.Op.SHOW,
            operand,
            show.loc,
            show.type
        )

        moment = self.next_moment()

        sym = self.new_sym(moment)

        operand.sym.last_used(moment)

        tac = Tac(
            sym,
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Int(self, i: Int) -> Tac:
        expr = IInt(
            i.value,
            i.loc,
            i.type,
        )

        sym = self.new_sym(value=i.value)

        tac = Tac(
            sym,
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Float(self, f: Float) -> Tac:
        expr = IFloat(
            f.value,
            f.loc,
            f.type,
        )

        sym = self.new_sym(value=f.value)

        tac = Tac(
            sym,
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Bool(self, b: Bool) -> Tac:
        expr = IBool(
            b.value,
            b.loc,
        )

        sym = self.new_sym(value=b.value)

        tac = Tac(
            sym,
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Str(self, s: Str) -> Tac:
        expr = IStr(
            s.value,
            s.loc,
            s.type,
        )

        sym = self.new_sym(value=s.value)

        tac = Tac(
            sym,
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

    def visit_Interpol(self, interpol: Interpol) -> Tac:
        synthetic_str = Str(interpol.left, interpol.loc)

        synthetic_show = Show(
            interpol.expr,
            interpol.expr.loc
        )

        expr = interpol.expr
        if not interpol.expr.type.is_str():
            expr = synthetic_show

        synthetic_first_concat = Concat(
            synthetic_str,
            BinOp.Op.CONCAT,
            expr,

            Tok.eof(),
            interpol.loc.union_hull(interpol.expr.loc)
        )

        synthetic_second_concat = Concat(
            synthetic_first_concat,
            BinOp.Op.CONCAT,
            interpol.next, 

            Tok.eof(),
            synthetic_first_concat.loc.union_hull(interpol.next.loc)
        )

        return self.visit(synthetic_second_concat)

    def visit_Nil(self, nil: Nil) -> Tac:
        expr = INil(
            nil.loc
        )

        sym = self.new_sym(value=Val.Nil())

        tac = Tac(
            sym,
            expr,
            expr.loc
        )

        self.ops.append(tac)
        return tac

