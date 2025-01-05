from nevec.ir.ir import *

from nevec.ast.visit import Visit

class Pass(Visit[Ir, None]):
    def __init__(self, syms: Syms):
        self.opts: List[Tac] = []

        self.syms: Syms = syms

        self.elims: int = 0

    def optimize(self, ir: List[Tac]) -> List[Tac]:
        if ir == []:
            return self.opts

        head = ir[0]
        self.visit(head)

        return self.optimize(ir[1:])

    def emit(self, *tac: Tac):
        self.opts.extend(tac)

    def elim_if_dead(self, sym: Sym, lend_name_to: Sym):
        if sym.uses > 0:
            return

        index = max(0, sym.first - self.elims)

        if len(self.opts) <= index:
            raise ValueError("attempt to eliminate symbol that does not exist")

        del self.opts[index]
        
        lend_name_to.rename(after=sym)

        del sym

        self.elims += 1

    def visit(self, node: Ir, *ctx: Tac):
        method_name = "visit_" + type(node).__name__
        method = getattr(self, method_name, None)

        if method is None:
            # i.e. this optimization pass doesn't involve type(node)
            self.emit(*ctx)
            return

        method(node, *ctx) 

    def visit_Tac(self, tac: Tac):
        self.visit(tac.expr, tac)
