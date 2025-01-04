from nevec.ir.ir import *

from nevec.opt.passes import Pass
from nevec.opt.const import ConstFold

class Opt:
    PASSES: List[type[Pass]] = [
        ConstFold
    ]

    def __init__(self, syms: Syms):
        self.syms: Syms = syms

    def optimize(
        self,
        ir: List[Tac],
        passes: Optional[List[type[Pass]]]=None
    ) -> List[Tac]:
        passes = passes if passes is not None else Opt.PASSES

        if passes == []:
            self.syms.cleanup()
            return ir

        opt_pass = passes[0](self.syms)

        opt_ir = opt_pass.optimize(ir)

        return self.optimize(opt_ir, passes[1:])
