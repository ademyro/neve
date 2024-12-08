from typing import List, Callable

from nevec.err.err import Err, Note, NoteType, Line, Suggestion
from nevec.lex.lex import Lex
from nevec.lex.tok import Loc, Tok, TokType, TokTypes

from nevec.parse.ast import *

class ParseErr:
    file_name: str
    lines: List[str]

    @staticmethod
    def setup(file_name: str, lines: List[str]):
        ParseErr.file_name = file_name
        ParseErr.lines = lines

    @staticmethod
    def unexpected_char(tok: Tok) -> Err:
        loc = tok.loc

        msg = tok.value if tok.value is not None else "invalid character"

        err = Err(
            ParseErr.file_name,
            ParseErr.lines,
            tok.value,
            loc
        ).show(
            Line(
               loc 
            ).add(
                Note(
                    NoteType.ERR,
                    loc,
                    f"here"
                )
            )
        )

        return err

    @staticmethod
    def unexpected_tok(tok: Tok, expected: TokType) -> Err:
        loc = tok.loc
        
        expected_lexeme = {
            lexeme 
            for lexeme in TokTypes.TOKS
            if TokTypes.TOKS[lexeme] == expected
        }

        expected_lexeme = list(expected_lexeme)[0]

        err = Err(
            ParseErr.file_name,
            ParseErr.lines,
            "unexpected token",
            loc
        ).show(
            Line(
               loc 
            ).add(
                Note(
                    NoteType.ERR,
                    loc,
                    f"expected '{expected_lexeme}' but found '{tok.lexeme}'"
                )
            )
        ).suggest(
            Suggestion(
                f"you can replace '{tok.lexeme}'",
                f"replaced '{tok.lexeme}' with '{expected_lexeme}'",
                loc,
                expected_lexeme
            )
        )

        return err

    @staticmethod
    def expected_tok(loc: Loc, expected: TokType) -> Err:
        expected_lexeme = {
            lexeme 
            for lexeme in TokTypes.TOKS
            if TokTypes.TOKS[lexeme] == expected
        }

        expected_lexeme = list(expected_lexeme)[0]

        err = Err(
            ParseErr.file_name,
            ParseErr.lines,
            f"'{expected_lexeme}' was expected, but found nothing",
            loc
        ).show(
            Line(
                loc
            ).add(
                Note(
                    NoteType.ERR,
                    loc,
                    f"expected '{expected_lexeme}'"
                )
            )
        ).suggest(
            Suggestion(
                f"however, you can insert it",
                f"added '{expected_lexeme}'",
                loc,
                expected_lexeme
            )
        )

        return err

    @staticmethod
    def expected_expr(tok: Tok) -> Err:
        loc = tok.loc

        err = Err(
            ParseErr.file_name,
            ParseErr.lines,
            "expected an expression",
            loc
        ).show(
            Line(
                loc,
                header_msg=f"'{tok.lexeme}' is not considered an expression"
            ).add(
                Note(
                    NoteType.ERR,
                    loc,
                    f"expected an expression, but found '{tok.lexeme}'"
                )
            )
        )

        return err

    @staticmethod
    def expected(what: str, tok: Tok) -> Err:
        loc = tok.loc

        err = Err(
            ParseErr.file_name,
            ParseErr.lines,
            f"expected {what}",
            loc
        ).show(
            Line(
                loc,
                header_msg=f"'{tok.lexeme}' is not considered an expression"
            ).add(
                Note(
                    NoteType.ERR,
                    loc,
                    f"not {what}"
                )
            )
        )

        return err

class Parse:
    def __init__(self, code: str):
        self.lex: Lex = Lex(code)
        self.curr: Tok = Tok.eof()
        self.prev: Tok = Tok.eof()

        self.had_err: bool = False
        self.panic_mode: bool = False

        self.file_name = self.lex.file_name
        self.lines = self.lex.lines

        ParseErr.setup(self.file_name, self.lines)

        self.advance()

    def show_err(self, err: Err):
        if self.panic_mode:
            return

        self.panic_mode = True 
        self.had_err = True
        err.print()

    def advance(self):
        self.prev = self.curr

        while True:
            self.curr = self.lex.next()

            if self.curr.type == TokType.NEWLINE:
                continue

            if self.curr.type != TokType.ERR:
                break

            # TODO: (re)implement proper error reporting
            self.show_err(ParseErr.unexpected_char(self.curr))

    def check(self, *type: TokType) -> bool:
        return self.curr.type in type
    
    def expect(self, type: TokType):
        if self.curr.type == type:
            self.advance()
            return

        if self.curr.type in (TokType.EOF, TokType.NEWLINE):
            self.show_err(ParseErr.expected_tok(self.curr.loc, type))
            return

        self.show_err(ParseErr.unexpected_tok(self.curr, type))

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

            case TokType.STR:
                return self.str_lit()
            
            case TokType.INTERPOL:
                return self.interpol()

        self.show_err(ParseErr.expected_expr(tok))
        return Expr(Types.UNKNOWN)

    def int_lit(self) -> Int:
        # TODO: allow hexadecimal, binary, and octal integers
        # and implement bounds checking for integers (LONG_MIN, LONG_MAX)
        tok = self.consume()

        value = int(tok.lexeme)

        return Int(value)

    def float_lit(self) -> Float:
        tok = self.consume()

        value = float(tok.lexeme)

        return Float(value)
    
    def str_lit(self) -> Str:
        tok = self.consume()

        value = tok.lexeme
        raw_str = Str.trim_quotes(value)

        return Str(raw_str)

    def interpol(self) -> Interpol:
        tok = self.consume() 

        value = tok.lexeme
        raw_str = Str.trim_quotes(value)

        interpol_expr = self.expr()

        # TODO: make sure the interpol_expr implements Show.

        next = None
        if self.check(TokType.INTERPOL):
            next = self.interpol() 
        else:
            if not self.check(TokType.STR):
                self.show_err(ParseErr.expected("a string", self.curr))

                return Interpol(raw_str, interpol_expr, Str.empty())

            next = self.str_lit()

        return Interpol(raw_str, interpol_expr, next)

    def grouping(self) -> Parens:
        self.advance()

        grouped = self.expr()
        
        self.expect(TokType.RPAREN)

        return Parens(grouped)
