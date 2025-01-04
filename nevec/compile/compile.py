from typing import BinaryIO, List, Any, Dict

from nevec.compile.peephole import Peephole
from nevec.ast.visit import Visit

from nevec.opcode.const import *
from nevec.opcode.instr import Instr
from nevec.opcode.emit import Emit

from nevec.ir.ir import *
from nevec.ir.reg import *

from nevec.err.report import Report

class Compile(Visit[Ir, None]):
    NEVE_MAGIC_NUMBER = 0xbadbed00
    NEVE_HEADER_SEPARATOR = 0x1c
    NEVE_EOF_PADDING_BYTE = 0xff

    def __init__(self, graph: InterferenceGraph):
        self.graph: InterferenceGraph = graph

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

        magic_number = Emit.encode_int(Compile.NEVE_MAGIC_NUMBER, 4)
        header_separator = Emit.encode_int(Compile.NEVE_HEADER_SEPARATOR, 1)
        eof_padding = [Emit.encode_int(Compile.NEVE_EOF_PADDING_BYTE, 1)] * 16

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

        debug_header_length = Emit.encode_int(self.debug_header_length, 2)

        self.debug_header_bytes = [
            debug_header_length,
            *self.debug_header_bytes
        ]

    def emit_first_bytes(self):
        source_file_path = Report.abs_file_path

        self.emit_debug(Emit.encode_int(len(source_file_path), 2))
        self.emit_debug(source_file_path.encode())

    def reg_of(self, sym: Sym) -> int:
        return self.graph.get_reg(sym)

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
        self.write(Emit.encode_int(i, size))

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
            self.emit_debug(Emit.encode_int(self.next_instr_offset, 4))
            self.emit_debug(Emit.encode_int(line, 4))

        self.next_instr_offset += 1

    def emit_const[T](self, const_type: type, value: T, reg: int, line: int):
        const = self.make_const(const_type, value)

        const_index = self.const_indices[const.id]

        # TODO: implement for Opcode.CONST_LONG
        self.emit(Instr(Opcode.CONST, reg, const_index), line)
    
    def compile(self, ir: List[Tac]):
        if ir == []:
            return 

        head = ir[0]

        self.visit(head) 

        self.compile(ir[1:])

    def visit_Tac(self, tac: Tac):
        sym  = tac.sym
        dest_reg = self.graph.get_reg(sym)

        self.visit(tac.expr, dest_reg)

    def visit_IRet(self, ret: IRet, dest_reg: int):
        self.emit(Instr(Opcode.RET, dest_reg), ret.loc.line)

    def visit_IUnOp(self, un_op: IUnOp, dest_reg: int):
        operand = self.reg_of(un_op.operand.sym)
        opcode = un_op.op.opcode()

        self.emit(Instr(opcode, dest_reg, operand), un_op.loc.line)

    def visit_IBinOp(self, bin_op: IBinOp, dest_reg: int):
        left = self.reg_of(bin_op.left.sym)
        right = self.reg_of(bin_op.right.sym)

        instr = Instr(
            bin_op.op.opcode(),

            dest_reg,
            left,
            right
        )

        self.emit(instr, bin_op.loc.line)

    def visit_IInt(self, i: IInt, dest_reg: int):
        line = i.loc.line

        match i.value:
            case 0:
                self.emit(Instr(Opcode.ZERO, dest_reg), line)
                return

            case 1:
                self.emit(Instr(Opcode.ONE, dest_reg), line)
                return

            case -1:
                self.emit(Instr(Opcode.MINUS_ONE, dest_reg), line)
                return

        self.emit_const(Num, i.value, dest_reg, line)

    def visit_IFloat(self, f: IFloat, dest_reg: int):
        self.emit_const(Num, f.value, dest_reg, f.loc.line)
    
    def visit_IBool(self, b: IBool, dest_reg: int):
        self.emit(
            Instr(
                Opcode.TRUE if b.value else Opcode.FALSE,
                dest_reg
            ),
            b.loc.line
        )

    def visit_IStr(self, s: IStr, dest_reg: int):
        self.emit_const(StrLit, s.value, dest_reg, s.loc.line)

    def visit_INil(self, nil: INil, dest_reg: int):
        self.emit(Instr(Opcode.NIL, dest_reg), nil.loc.line)
        
