from enum import auto, Enum
from dataclasses import dataclass
from typing import Dict

class TypeKind(Enum):
    UNKNOWN = auto()
    INT = auto()
    FLOAT = auto()
    BOOL = auto()
    NIL = auto()
    STR = auto()


@dataclass
class Type:
    kind: TypeKind
    name: str
    
    def __eq__(self, other: "Type"):
        return self.name == other.name


class Types:
    UNKNOWN = Type(TypeKind.UNKNOWN, "")
    INT = Type(TypeKind.INT, "Int")
    FLOAT = Type(TypeKind.FLOAT, "Float")
    BOOL = Type(TypeKind.BOOL, "Bool")
    NIL = Type(TypeKind.NIL, "Nil")
    STR = Type(TypeKind.STR, "Str")
    


class TypeTable:
    def __init__(self):
        self.types: Dict[str, Type] = {
            "Int": Types.INT,
            "Float": Types.FLOAT,
            "Bool": Types.BOOL,
            "Nil": Types.NIL,
            "Str": Types.STR
        }
        
    def register(self, type: Type):
        self.types[type.name] = type
