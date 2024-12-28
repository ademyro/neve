import struct

from typing import Self, List
from enum import auto, Enum

class ValType(Enum):
    NUM = auto()
    BOOL = auto()
    NIL = auto() 
    OBJ = auto()


class ObjType(Enum):
    STR = auto()


class Const[T]:
    def __init__(self, value: T, id: int):
        self.value = value
        self.id: int = id
        
    def emit_int(self, data: int, size: int) -> bytes:
        return data.to_bytes(size, byteorder="little")

    def emit_type(self, type: ValType | ObjType) -> bytes:
        return self.emit_int(type.value - 1, 1)

    def emit(self) -> List[bytes]:
        ...

    def __eq__(self, other: Self) -> bool:
        _ = other

        ...


class Num(Const[float]):
    def emit(self) -> List[bytes]:
        return [
            self.emit_type(ValType.NUM),
            struct.pack("<d", self.value)
        ]

    def __eq__(self, other: Const) -> bool:
        return (
            isinstance(other, Num) and
            self.value == other.value
        )

    def __repr__(self) -> str:
        return str(self.value)

class StrLit(Const[str]):
    def emit(self) -> List[bytes]:
        return [
            self.emit_type(ValType.OBJ),
            self.emit_type(ObjType.STR),
            self.emit_int(len(self.value), 4),
            self.value.encode()
        ]

    def __eq__(self, other: Const) -> bool:
        return (
            isinstance(other, StrLit) and
            self.value == other.value
        )

    def __repr__(self) -> str:
        return self.value
