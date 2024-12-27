import struct

from typing import BinaryIO, List, Any, Dict

from nevec.ast.visit import Visit
from nevec.compile.const import *
from nevec.ir.ir import *
from nevec.err.report import Report

class Compile(Visit[Ir, None]):
    NEVE_MAGIC_NUMBER = 0xbadbed00
    NEVE_HEADER_SEPARATOR = 0x1c
    NEVE_EOF_PADDING_BYTE = 0xff

    def __init__(self):
        self.const_header_bytes: List[bytes] = []
        self.debug_header_bytes: List[bytes] = []
        self.opcodes: List[bytes] = []

        self.next_const_index: int = 0
        self.next_instr_offset: int = 0

        self.debug_header_length: int = 1

        self.const_ids: Dict[Any, int] = {}
        self.const_indices: Dict[int, int] = {}

        self.emit_first_bytes()
    
    def output(self, to: BinaryIO):
        self.finalize()

        magic_number = self.encode_int(Compile.NEVE_MAGIC_NUMBER, 4)
        header_separator = self.encode_int(Compile.NEVE_HEADER_SEPARATOR, 1)
        eof_padding = [self.encode_int(Compile.NEVE_EOF_PADDING_BYTE, 1)] * 16

        byte_list = [
            magic_number,
            *self.const_header_bytes,
            header_separator,
            *self.debug_header_bytes, 
            header_separator,
            *self.opcodes,
            *eof_padding 
        ] 

        # using list() because silly Python doesn't immediately interpret
        # map() objects
        list(map(lambda b: to.write(b), byte_list))

    def finalize(self):
        last_line = self.debug_header_bytes[-1]
        self.emit_byte(Opcode.RET, int.from_bytes(last_line))

        debug_header_length = self.encode_int(self.debug_header_length, 2)

        self.debug_header_bytes = [
            debug_header_length,
            *self.debug_header_bytes
        ]

    def emit_first_bytes(self):
        source_file_path = Report.abs_file_path

        self.emit_debug(self.encode_int(len(source_file_path), 2))
        self.emit_debug(source_file_path.encode())

    def encode_int(self, data: int, size: int) -> bytes:
        return data.to_bytes(size, byteorder="little")

    def make_const[T](self, const_type: type[Const], value: T) -> Const:
        if value in self.const_ids:
            id = self.const_ids[value]
            return const_type(value, id)

        next_id = len(self.const_ids)
        const = const_type(value, next_id)

        self.const_ids[value] = next_id
         
        self.const_indices[const.id] = self.next_const_index
        self.next_const_index += 1 

        const_bytes = const.emit()
        self.const_header_bytes.extend(const_bytes)

        return const

    def emit(self, data: bytes):
        self.opcodes.append(data)

    def emit_debug(self, data: bytes):
        self.debug_header_bytes.append(data)

        self.debug_header_length += len(data)

    def emit_int(self, i: int, size: int):
        self.emit(self.encode_int(i, size))

    def emit_str(self, s: str):
        self.emit(s.encode())

    def emit_byte(
        self,
        byte: Opcode | int,
        line: int,
    ):
        last_line = int.from_bytes(self.debug_header_bytes[-1])

        byte = byte if not isinstance(byte, Opcode) else byte.value - 1

        self.emit_int(byte, 1)

        if line != last_line:
            self.emit_debug(self.encode_int(self.next_instr_offset, 4))
            self.emit_debug(self.encode_int(line, 4))

        self.next_instr_offset += 1

    def emit_bytes(
        self,
        a: int | Opcode,
        b: int | Opcode,
        line: int, 
    ):
        self.emit_byte(a, line)
        self.emit_byte(b, line)

    def emit_const[T](self, const_type: type, value: T, line: int):
        const = self.make_const(const_type, value)

        const_index = self.const_indices[const.id]

        # TODO: implement for Opcode.CONST_LONG
        self.emit_bytes(Opcode.CONST, const_index, line)

    def visit_IUnOp(self, un_op: IUnOp):
        self.visit(un_op.operand)

        self.emit_byte(un_op.op.opcode(), un_op.loc.line)

    def visit_IBinOp(self, bin_op: IBinOp):
        self.visit(bin_op.left)
        self.visit(bin_op.right)

        self.emit_byte(bin_op.op.opcode(), bin_op.loc.line)

    def visit_IInt(self, i: IInt):
        line = i.loc.line

        match i.value:
            case 0:
                self.emit_byte(Opcode.ZERO, line)
                return

            case 1:
                self.emit_byte(Opcode.ONE, line)
                return

            case -1:
                self.emit_byte(Opcode.MINUS_ONE, line)
                return

        self.emit_const(Num, i.value, line)

    def visit_IFloat(self, f: IFloat):
        self.emit_const(Num, f.value, f.loc.line)
    
    def visit_IBool(self, b: IBool):
        self.emit_byte(
            Opcode.TRUE if b.value else Opcode.FALSE,
            b.loc.line
        )
       
    def visit_IStr(self, s: IStr):
        self.emit_const(StrLit, s.value, s.loc.line)

    def visit_IInterpol(self, interpol: IInterpol):
        raise NotImplementedError()
        # def emit(next: Ir) -> int:
        #     if not isinstance(next, IInterpol):
        #         if not isinstance(next, IStr):
        #             return -1
        # 
        #         self.emit_const(StrLit, next.value, next.loc.line)
        #         return 0
        # 
        #     self.emit_const(StrLit, next.left, next.loc.line)
        #     self.visit(next.expr)
        #     
        #     return 1 + emit(next.next)
        # 
        # times = emit(interpol) 
        # 
        # self.emit_bytes(Opcode.INTERPOL, times, interpol.loc.line)

    def visit_INil(self, nil: INil):
        self.emit_byte(Opcode.NIL, nil.loc.line)
        
