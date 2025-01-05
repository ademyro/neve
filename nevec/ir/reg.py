from typing import Dict, Self, List, Optional, Tuple

from enum import auto, Enum

from nevec.ir.sym import Sym

class Vertex:
    def __init__(self, name: str, value: int):
        self.name: str = name
        self.value: int = value
        
        self.reg: int = -1

        self.adjacent: List[Vertex] = []
        self.unavailable_regs: List[int] = []

    def next_reg(self, reg=0):
        if self.unavailable_regs == []:
            self.unavailable_regs = [v.reg for v in self.adjacent]

        if reg not in self.unavailable_regs:
            self.reg = reg
            return

        self.next_reg(reg + 1)

    def connect(self, to: Self):
        self.adjacent.append(to)
        to.adjacent.append(self)

    def assign(self, reg: int):
        self.reg = reg

    def __repr__(self) -> str:
        return f"{self.name} r{self.reg}"

class InterferenceGraph:
    def __init__(self, syms: List[Sym]):
        self.regs: Dict[str, Vertex] = {}

        assert list(filter(lambda s: s.lifetime is None, syms)) == []

        self.draw_edges(based_on=syms)
        self.assign_registers()

    def draw_edges(self, based_on: List[Sym]):
        def check(syms: List[Tuple[int, Sym]]):
            def each(
                sym: Tuple[int, Sym],
                other_syms: List[Tuple[int, Sym]]
            ):
                if other_syms == []:
                    return

                other = other_syms[0]

                sym_itself = sym[1]
                other_sym = other[1]
                
                other_lifetime = other_sym.lifetime
                sym_lifetime = sym_itself.lifetime 

                # this is silly too
                assert sym_lifetime is not None and other_lifetime is not None

                if sym_lifetime.intersects_with(other_lifetime):
                    other_vertex = self.regs[other_sym.full_name]
                    vertex = self.regs[sym_itself.full_name]

                    vertex.connect(to=other_vertex)

                each(sym, other_syms[1:])

            if syms == []:
                return

            head = syms[0]

            each(head, syms[1:])
            check(syms[1:])

        self.regs = {
            s.full_name: Vertex(s.full_name, i)
            for i, s in enumerate(based_on)
        }

        check(list(enumerate(based_on)))

    def assign_registers(self):
        vertices = list(self.regs.values())

        # using list() because silly Python doesn't immediately interpretet
        # map() expressions
        list(map(Vertex.next_reg, vertices))

        self.regs = {v.name: v for v in vertices}

    def get_reg(self, sym: Sym) -> int:
        return self.regs[sym.full_name].reg
