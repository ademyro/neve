from typing import List

class Emit:
    @staticmethod
    def encode_int(value: int, size: int) -> bytes:
        return value.to_bytes(size, byteorder="little")

    @staticmethod
    def integer(value: int, size: int) -> List[bytes]:
        return [Emit.encode_int(value, size)] 

    @staticmethod
    def str_lit(s: str) -> List[bytes]:
        return [s.encode()]

    @staticmethod
    def many(*data: int) -> List[bytes]:
        bytes = list(data)

        if bytes == []:
            return []

        return Emit.integer(bytes[0], size=1) + Emit.many(*bytes[1:])
        
