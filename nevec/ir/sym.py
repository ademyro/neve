from typing import Optional

type Moment = int

class Lifetime:
    def __init__(self, first: Moment, last: Optional[Moment]=None):
        self.first: Moment = first
        self.last: Optional[Moment] = last
    
    def is_valid_in(self, moment: Moment) -> bool:
        assert self.last is not None

        return self.first >= moment and self.last <= moment


class Sym:
    def __init__(self, name: str, index: int, moment: Moment):
        self.name: str = name
        self.index: int = index
        self.first: Moment = moment

        self.full_name = self.name + str(self.index)

        self.lifetime: Optional[Lifetime] = None

    def last_used(self, last: Moment):
        self.lifetime = Lifetime(self.first, last)

    def is_alive_in(self, moment: Moment) -> bool:
        assert self.lifetime is not None

        return self.lifetime.is_valid_in(moment)

    def __repr__(self) -> str:
        return self.full_name


class NamelessSym[T](Sym):
    def __init__(self, value: T, moment: Moment):
        self.value = value
        self.first: Moment = moment

        self.lifetime: Optional[Lifetime] = None

    def __repr__(self) -> str:
        return str(self.value)
