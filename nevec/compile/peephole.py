from typing import List

from nevec.opcode.instr import Instr

class Peephole:
    def __init__(self):
        self.peephole: List[Instr] = []

        self.has_flushed: bool = False
        self.flushed: List[bytes] = []
    
    def flush(self):
        # i needed so much mental gymnastics for something so silly
        list_of_lists_of_bytes = map(Instr.emit, self.peephole)

        self.flushed = [b for r in list_of_lists_of_bytes for b in r]

        self.has_flushed = True
        self.peephole = []

    def push(self, instr: Instr):
        self.peephole.append(instr)
        
        # we don't have any peephole optimizations yet--let's always flush
        self.flush()
