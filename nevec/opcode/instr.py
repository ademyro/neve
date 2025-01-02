from typing import List

from nevec.opcode.opcode import *
from nevec.opcode.emit import Emit

class Instr:
    def __init__(self, opcode: Opcode, *operands: int):
        self.opcode: Opcode = opcode
        self.operands: List[int] = list(operands)

    def emit(self) -> List[bytes]:
        return Emit.many(
            self.opcode.raw(),
            *self.operands
        )  
