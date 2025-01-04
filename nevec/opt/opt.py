from nevec.ir.ir import *

from nevec.opt.passes import Pass
from nevec.opt.const import ConstFold

class Opt:
    PASSES: List[Pass] = [
        ConstFold()
    ]

    @staticmethod
    def optimize(
        ir: List[Tac],
        passes: Optional[List[Pass]]=None
    ) -> List[Tac]:
        passes = passes if passes is not None else Opt.PASSES

        if passes == []:
            return ir

        opt_pass = passes[0]

        opt_ir = opt_pass.optimize(ir)

        return Opt.optimize(opt_ir, passes[1:])
