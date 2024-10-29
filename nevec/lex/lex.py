import string

from typing import List, Optional

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

class Lex:
    MAX_INTERPOL_DEPTH = 255

    def __init__(self, code: str):
        self.code: CharQueue = CharQueue(code)
        self.loc: Loc = Loc.new()
        self.char: Optional[str] = None
        self.lexeme: List[str] = []

        self.interpol_depth: int = 0

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

    def new_tok(self, type: TokType) -> Tok:
        return Tok(type, "".join(self.lexeme), self.loc.copy())

    def err(self, msg: str) -> Tok:
        return Tok(TokType.ERR, "".join(self.lexeme), self.loc.copy(), msg)

    def next(self) -> Tok:
        if self.char is None:
            self.sync()
            return self.new_tok(TokType.EOF)

        self.skip_ws()
        self.sync()
        
        if self.on_digit():
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

            self.sync()

            return self.string()

        if self.char == '\n':
            self.advance()
            self.loc.newline()
            return self.new_tok(TokType.NEWLINE)
        
        return self.simple_tok()
        
    def skip_ws(self):
        if not self.on_ws():
            return
        
        if self.char == "#":
            self.skip_comment()
        
        self.advance()
        self.skip_ws()

    def skip_comment(self):
        if self.char == '\n':
            self.loc.newline()
            return
        
        self.advance()
        
        self.skip_comment()

    def simple_tok(self) -> Tok:
        next_char = self.peek()
        current_char = self.char

        seq = (
            current_char + next_char
            if next_char is not None
            else current_char
        )

        tok_type = TokType.match(seq) 
        
        if tok_type is not None:
            self.advance()
            self.advance()

            return self.new_tok(tok_type)

        # otherwise, it's likely one char long
        new_tok_type = TokType.match(current_char)

        if new_tok_type is None:
            self.advance()
            return self.err("unexpected character")

        self.advance()
        return self.new_tok(new_tok_type)

    def number(self) -> Tok:
        found_dot = False

        while not self.is_at_end() and self.on_digit():
            if self.char == ".":
                if not self.found_dot:
                    found_dot = True
                else:
                    break
            
            self.advance()

        lexeme = "".join(self.lexeme)
        tok = Tok(
            TokType.FLOAT if found_dot else TokType.INT,

            lexeme,
            self.loc.copy(),

            float(lexeme) if found_dot else int(lexeme)
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

    def string(self):
        self.advance()

        while self.char != '"' and not self.is_at_end():
            self.advance()

            if (
                self.char == "#" and
                self.peek() == "{" 
            ):
                return self.interpol()
            
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

    def on_ws(self):
        return self.char in " \r\t#"
    
    def on_digit(self):
        return self.char in "1234567890."

    def on_alpha(self):
        return self.char in string.ascii_letters or self.char == '_'
