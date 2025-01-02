from typing import BinaryIO, List, Any, Dict

from nevec.compile.peephole import Peephole
from nevec.ast.visit import Visit

from nevec.opcode.const import *
from nevec.opcode.instr import Instr

from nevec.ir.ir import *
from nevec.ir.reg import *

from nevec.err.report import Report

class Compile(Visit[Ir, Reg]):
    NEVE_MAGIC_NUMBER = 0xbadbed00
    NEVE_HEADER_SEPARATOR = 0x1c
    NEVE_EOF_PADDING_BYTE = 0xff

    def __init__(self):
        self.const_header_bytes: List[bytes] = []
        self.debug_header_bytes: List[bytes] = []
        self.opcodes: List[bytes] = []

        self.peephole: Peephole = Peephole()

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
        self.emit(Instr(Opcode.RET, 0), int.from_bytes(last_line))

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

    def write(self, *data: bytes):
        self.opcodes += list(data)

    def emit_debug(self, data: bytes):
        self.debug_header_bytes.append(data)

        self.debug_header_length += len(data)

    def emit_int(self, i: int, size: int):
        self.write(self.encode_int(i, size))

    def emit_str(self, s: str):
        self.write(s.encode())

    def emit(
        self,
        instr: Instr,
        line: int,
    ):
        last_line = int.from_bytes(self.debug_header_bytes[-1])

        self.peephole.push(instr)
        if self.peephole.has_flushed:
            self.write(*self.peephole.flushed)

        if line != last_line:
            self.emit_debug(self.encode_int(self.next_instr_offset, 4))
            self.emit_debug(self.encode_int(line, 4))

        self.next_instr_offset += 1

    def emit_const[T](self, const_type: type, value: T, reg: Reg, line: int):
        const = self.make_const(const_type, value)

        const_index = self.const_indices[const.id]

        # TODO: implement for Opcode.CONST_LONG
        self.emit(Instr(Opcode.CONST, reg.emit(), const_index), line)

    def visit_IUnOp(self, un_op: IUnOp) -> Reg:
        operand = self.visit(un_op.operand)
        output = un_op.reg

        opcode = un_op.op.opcode()

        self.emit(Instr(opcode, output.emit(), operand.emit()), un_op.loc.line)

        return output 

    def visit_IBinOp(self, bin_op: IBinOp) -> Reg:
        left = self.visit(bin_op.left)
        right = self.visit(bin_op.right)

        output = bin_op.reg

        instr = Instr(
            bin_op.op.opcode(),

            output.emit(),
            left.emit(),
            right.emit()
        )

        self.emit(instr, bin_op.loc.line)
        
        return output

    def visit_IInt(self, i: IInt) -> Reg:
        reg = i.reg
        line = i.loc.line

        match i.value:
            case 0:
                self.emit(Instr(Opcode.ZERO, reg.emit()), line)
                return reg

            case 1:
                self.emit(Instr(Opcode.ONE, reg.emit()), line)
                return reg

            case -1:
                self.emit(Instr(Opcode.MINUS_ONE, reg.emit()), line)
                return reg

        self.emit_const(Num, i.value, reg, line)
        return reg

    def visit_IFloat(self, f: IFloat) -> Reg:
        self.emit_const(Num, f.value, f.reg, f.loc.line)

        return f.reg
    
    def visit_IBool(self, b: IBool) -> Reg:
        self.emit(
            Instr(
                Opcode.TRUE if b.value else Opcode.FALSE,
                b.reg.emit(),
            ),
            b.loc.line
        )

        return b.reg
       
    def visit_IStr(self, s: IStr):
        self.emit_const(StrLit, s.value, s.reg, s.loc.line)

        return s.reg

    def visit_INil(self, nil: INil):
        self.emit(Instr(Opcode.NIL, nil.reg.emit()), nil.loc.line)

        return nil.reg
        
