from typing import List, Dict, Optional, Self, Tuple

type Moment = int

class Lifetime:
    def __init__(self, first: Moment, last: Optional[Moment]=None):
        self.first: Moment = first
        self.last: Optional[Moment] = last

    def intersects_with(self, other: Self) -> bool:
        assert self.last is not None and other.last is not None

        return (
            (self.first >= other.first and self.last <= other.last) or
            (other.first >= self.first and other.last <= self.last)
        )
    
    def is_valid_in(self, moment: Moment) -> bool:
        assert self.last is not None

        return self.first >= moment and self.last <= moment

    def __repr__(self) -> str:
        return f"({self.first}, {self.last})"


class Sym[T]:
    def __init__(
        self,
        name: str,
        index: int,
        moment: Moment,
        value: Optional[T]=None
    ):
        self.name: str = name
        self.index: int = index
        self.first: Moment = moment

        self.value: Optional[T] = value

        self.full_name = self.name + str(self.index)
        self.uses: int = 0

        self.lifetime: Optional[Lifetime] = None

    def rename(self, after: Self):
        if self.index < after.index:
            return

        self.index = after.index
        self.full_name = self.name + str(self.index)

    def propagate(self):
        self.uses -= 1

    def last_used(self, last: Moment):
        self.uses += 1

        self.lifetime = Lifetime(self.first, last)

    def is_alive_in(self, moment: Moment) -> bool:
        assert self.lifetime is not None

        return self.lifetime.is_valid_in(moment)

    def __repr__(self) -> str:
        return self.full_name


class Syms:
    def __init__(self):
        self.syms: Dict[str, Sym] = {}

    def new_sym[T](
        self,
        moment: Moment,
        name: str="t",
        value: Optional[T]=None
    ) -> Sym:
        name, index = self.next_available_name(name)

        sym = Sym(name, index, moment, value)

        self.syms[sym.full_name] = sym

        return sym

    def next_available_name(self, name: str, index: int=0) -> Tuple[str, int]:
        full_name = name + str(index)

        if full_name not in self.syms.keys():
            return name, index

        return self.next_available_name(name, index + 1)

    def cleanup(self):
        self.syms = {n: s for n, s in self.syms.items() if s.uses > 0}

    def values(self) -> List[Sym]:
        return list(self.syms.values())
