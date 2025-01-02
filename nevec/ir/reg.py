from typing import Dict, List, Optional

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

    def freeze(self):
        self = self.copy()

    def emit(self) -> int:
        return self.index

    def __repr__(self) -> str:
        return f"r{self.index}"

class RegManager:
    def __init__(self):
        self.regs: Dict[str, Reg] = {}
        self.available: List[Reg] = []
    
    def assign(self, name: str):
        self.regs[name] = self.next()

    def get(self, name: str):
        return self.regs[name]

    def next(self, regs: Optional[List[Reg]]=None) -> Reg:
        regs = regs if regs is not None else self.available 

        if regs == []:
            index = len(self.regs)
            new_reg = Reg(index, Reg.State.TEMP)

            self.available.append(new_reg)

            return new_reg

        head = regs[0] 
        
        if head.state == Reg.State.TEMP:
            return head

        return self.next(regs[1:])

    def next_index(self) -> int:
        return len(self.regs)
