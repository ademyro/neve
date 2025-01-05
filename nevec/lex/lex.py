import string

from typing import List, Optional

from nevec.err.report import Report
from nevec.lex.tok import Loc, TokType, Tok

class CharQueue:
    def __init__(self, chars: str):
        # yeah list(reversed(list(chars))) is definitely weird, but apparently,
        # reversed(list(chars))'s type is a list_reverseiterator, and that 
        # type doesn't have the .pop() method for some reason.
        self.chars: List[str] = list(reversed(list(chars)))
        self.items_left: int = len(chars)

    def pop(self) -> Optional[str]:
        if self.items_left == 0:
            return None 

        self.items_left -= 1
        return self.chars.pop()

    def peek(self) -> Optional[str]:
        if self.items_left < 2:
            return None

        return self.chars[-1]

    def __repr__(self) -> str:
        return "".join(self.chars)

class Lex:
    MAX_INTERPOL_DEPTH = 255
    DIGITS = "1234567890"
    WS = " \r\t"

    def __init__(self, code: str, file_name="test.neve"):
        self.code: CharQueue = CharQueue(code)
        self.file_name: str = file_name
        self.loc: Loc = Loc.new()
        self.char: Optional[str] = None
        self.lexeme: List[str] = []

        self.interpol_depth: int = 0
        self.in_interpol: bool = False

        self.lines: List[str] = code.split("\n")

        Report.setup(file_name, self.lines)

        self.advance()

    def advance(self):
        if self.char is not None:
            self.lexeme.append(self.char)

        self.char = self.code.pop() 

        self.loc.advance()

    def is_at_end(self) -> bool:
        return self.char is None

    def sync(self):
        self.lexeme = []
        self.loc.sync()

    def peek(self) -> Optional[str]:
        return self.code.peek()

    def discard_all(self, chars: str):
        if self.is_at_end() or self.char not in chars:
            self.sync()
            return
        
        self.advance()
        self.discard_all(chars)

    def new_tok(self, type: TokType) -> Tok:
        return Tok(type, "".join(self.lexeme), self.loc.copy())

    def err(self, msg: str) -> Tok:
        return Tok(TokType.ERR, "".join(self.lexeme), self.loc.copy(), msg)

    def next(self) -> Tok:
        self.skip_ws()
        self.sync()

        if self.char is None:
            self.sync()
            return self.new_tok(TokType.EOF)

        if self.in_interpol:
            return self.string(capture_first_char=False)
        
        if self.on_digit() or self.on_float():
            return self.number()

        if self.on_alpha():
            return self.id()

        if self.char == '"':
            return self.string()

        if self.char == '}':
            if self.interpol_depth == 0:
                self.advance()
                return self.err("'}' outside string interpolation")
            
            self.interpol_depth -= 1
            self.in_interpol = True

            return self.new_tok(TokType.INTERPOL_SEP)

        if self.char == '\n':
            self.advance()
            self.loc.newline()
            return self.new_tok(TokType.NEWLINE)
        
        return self.simple_tok()
        
    def skip_ws(self):
        if self.char == "#":
            self.skip_comment()
        
        self.discard_all(Lex.WS)

    def skip_comment(self):
        if self.char == '\n':
            self.advance()
            self.loc.newline()
            return
        
        if self.is_at_end():
            return
        
        self.advance()
        self.skip_comment()

    def simple_tok(self) -> Tok:
        next_char = self.peek()
        current_char = self.char

        if next_char is not None:
            seq = current_char + next_char

            tok_type = TokType.match(seq) 
            
            if tok_type is not None:
                self.advance()
                self.advance()

                return self.new_tok(tok_type)

        # otherwise, it's likely one char long
        new_tok_type = TokType.match(current_char)

        if new_tok_type is None:
            self.advance()
            return self.err(f"invalid character")

        self.advance()
        return self.new_tok(new_tok_type)

    def number(self) -> Tok:
        while self.on_digit():
            self.advance()             

        is_float = self.on_float()

        if is_float:
            self.advance()

            while self.on_digit():
                self.advance()
        
        # still on_float() => something like 1.2.3
        if self.on_float():
            # skip the "."
            self.advance()

            self.discard_all(Lex.DIGITS)
            return self.err("a float may not have more than one decimal portion")

        lexeme = "".join(self.lexeme)
         
        tok = Tok(
            TokType.FLOAT if is_float else TokType.INT,
            
            lexeme,
            self.loc.copy(),

            float(lexeme) if is_float else int(lexeme)
        )

        return tok

    def id(self):
        while self.on_alpha():
            self.advance()

        lexeme = "".join(self.lexeme)
        keyword_type = TokType.match_keyword(lexeme)
        
        return self.new_tok(
            TokType.ID 
            if keyword_type is None 
            else keyword_type
        )

    def string(self, capture_first_char=True):
        self.advance()

        if not capture_first_char:
            self.sync()

        if self.in_interpol:
            self.in_interpol = False

        while self.char != '"' and not self.is_at_end():
            if (
                self.char == "#" and
                self.peek() == "{" 
            ):
                return self.interpol()

            self.advance()
            
            if self.char == "\n":
                self.loc.newline()

        if self.is_at_end():
            return self.err("unterminated string")
        
        self.advance()

        return self.new_tok(TokType.STR)

    def interpol(self):
        if self.interpol_depth == Lex.MAX_INTERPOL_DEPTH:
            return self.err("maximum string interpolation depth exceeded")

        self.interpol_depth += 1

        interpol_tok = self.new_tok(TokType.INTERPOL)

        self.sync()

        self.advance()
        self.advance()

        if self.char == "}":
            # i.e. empty interpolation
            self.advance()

            return self.err("empty string interpolation")

        return interpol_tok 

    def on_ws(self) -> bool:
        return self.char in Lex.WS or self.char == "#"
    
    def on_digit(self) -> bool:
        return self.is_digit(self.char)
    
    def on_float(self) -> bool:
        return self.char == "." and self.is_digit(self.peek())

    def on_alpha(self) -> bool:
        return (
            self.char is not None and 
            (
                self.char in string.ascii_letters or 
                self.char == '_'
            )
        )

    def is_digit(self, char: str) -> bool:
        return char is not None and char in Lex.DIGITS
