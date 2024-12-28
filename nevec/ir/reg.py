from typing import Self, List, Optional

from enum import auto, Enum

class Reg:
    class State(Enum):
        TEMP = auto()
        NECESSARY = auto()
        ALLOCATED = auto()

    
    def __init__(self, index: int, state: State):
        self.index: int = index
        self.state: Reg.State = state

    def allocate(self):
        self.state = Reg.State.ALLOCATED

    def copy(self) -> "Reg":
        return Reg(self.index, self.state)


class RegManager:
    def __init__(self):
        self.regs: List[Reg] = []

    def next(self, regs: Optional[List[Reg]]=None) -> Reg:
        regs = regs if regs is not None else self.regs 

        if regs == []:
            index = len(self.regs)
            new_reg = Reg(index, Reg.State.TEMP)

            self.regs.append(new_reg)

            return new_reg

        head = regs[0] 
        
        if head.state == Reg.State.TEMP:
            return head

        return self.next(regs[1:])
