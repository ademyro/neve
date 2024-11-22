from typing import Callable

from nevec.lex.lex import Lex
from nevec.lex.tok import Tok, TokType

from nevec.parse.ast import *

class Parse:
    def __init__(self, code: str):
        self.lex: Lex = Lex(code)
        self.curr: Tok = Tok.eof()
        self.prev: Tok = Tok.eof()
        self.had_err: bool = False

        self.advance()

    def advance(self):
        self.prev = self.curr

        while True:
            self.curr = self.lex.next()

            if self.curr.type == TokType.NEWLINE:
                continue

            if self.curr.type != TokType.ERR:
                break

            # TODO: (re)implement proper error reporting
            print("advance: unexpected token")

    def check(self, *type: TokType) -> bool:
        return self.curr.type in type
    
    def expect(self, type: TokType):
        if self.curr.type == type:
            self.advance()
            return
        
        print("expect", type, ": unexpected token")

    def consume(self) -> Tok:
        tok = self.curr
        self.advance()

        return tok
    
    def parse(self) -> Ast:
        ast = self.expr()

        if self.had_err:
            del ast
            return Ast(Types.UNKNOWN)
        
        return ast

    def expr(self) -> Expr:
        return self.bit_or()

    def bit_or(self) -> Expr:
        return self.bin_op(self.bit_xor, TokType.BIT_OR)
    
    def bit_xor(self) -> Expr:
        return self.bin_op(self.bit_and, TokType.BIT_XOR)
    
    def bit_and(self) -> Expr:
        return self.bin_op(self.equality, TokType.BIT_AND)

    def equality(self) -> Expr:
        return self.bin_op(self.comparison, TokType.EQ, TokType.NEQ) 

    def comparison(self) -> Expr:
        return self.bin_op(
            self.bit_shift, 
            TokType.GT, 
            TokType.GTE, 
            TokType.LT, 
            TokType.LTE
        )

    def bit_shift(self) -> Expr:
        return self.bin_op(self.term, TokType.SHL, TokType.SHR)

    def term(self) -> Expr:
        return self.bin_op(self.factor, TokType.PLUS, TokType.MINUS)

    def factor(self) -> Expr:
        return self.bin_op(self.unary, TokType.STAR, TokType.SLASH)
    
    def bin_op(self, fun: Callable, *ops: TokType) -> Expr:
        left = fun()

        while self.check(*ops):
            op = self.consume()

            right = fun()

            left = BinOp(left, BinOp.from_tok(op), right, op)

        return left

    def unary(self) -> Expr:
        if not self.check(TokType.MINUS, TokType.NOT):
            return self.primary()
        
        op = self.consume()
        operand = self.unary()

        unop_type = (
            UnOp.UnOpType.NEG
            if op.type == TokType.MINUS
            else UnOp.UnOpType.NOT
        )

        return UnOp(unop_type, operand)

    def primary(self) -> Expr:
        tok = self.curr

        match tok.type:
            case TokType.INT:
                return self.int_lit()

            case TokType.FLOAT:
                return self.float_lit()

            case TokType.TRUE | TokType.FALSE:
                self.advance()
                return Bool(tok.type == TokType.TRUE)
            
            case TokType.NIL:
                self.advance()
                return Nil()

            case TokType.LPAREN:
                return self.grouping()

        print("primary: unexpected token", tok)
        return Expr(Types.UNKNOWN)

    def int_lit(self) -> Expr:
        # TODO: allow hexadecimal, binary, and octal integers
        # and implement bounds checking for integers (LONG_MIN, LONG_MAX)
        tok = self.consume()

        value = int(tok.lexeme)

        return Int(value)

    def float_lit(self) -> Expr:
        tok = self.consume()

        value = float(tok.lexeme)

        return Float(value)

    def grouping(self):
        self.advance()

        grouped = self.expr()
        
        self.expect(TokType.RPAREN)

        return Parens(grouped)
