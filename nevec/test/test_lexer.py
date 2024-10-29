import test

from typing import List

from nevec.lex.lex import Lex
from nevec.lex.tok import Tok, TokType, Loc

def all_toks(lex: Lex) -> List[Tok]:
    tok = lex.next()

    if tok.type == TokType.EOF:
        return [tok]
    
    return [tok] + all_toks(lex)

def all_similar(left: List[Tok], right: List[TokType]) -> bool:
    assert len(left) == len(right)

    for i, tok in enumerate(left):
        assert tok.type == right[i]
    
    return True


class TestLex:
    def test_one(self):
        input = "let my_name = \"Name\"\nputs my_name == \"Name\"" 

        lex = Lex(input)

        toks = all_toks(lex) 

        assert all_similar(
            toks,
            [
                TokType.LET,
                TokType.ID,
                TokType.ASSIGN,
                TokType.STR,
                TokType.NEWLINE,
                TokType.PUTS,
                TokType.ID,
                TokType.EQ,
                TokType.STR,
                TokType.EOF
            ]
        )

    def test_invalid(self):
        input = "let @ = 42"

        lex = Lex(input)
        toks = all_toks(lex)

        assert all_similar(
            toks,
            [
                TokType.LET,
                TokType.ERR,
                TokType.ASSIGN,
                TokType.INT,
                TokType.EOF
            ]
        )
    
    def test_complex(self):
        input = "x + y * (z - 1)"

        lex = Lex(input)
        toks = all_toks(lex)

        assert all_similar(
            toks,
            [
                TokType.ID,
                TokType.PLUS,
                TokType.ID,
                TokType.STAR,
                TokType.LPAREN,
                TokType.ID,
                TokType.MINUS,
                TokType.INT,
                TokType.RPAREN,
                TokType.EOF
            ]
        )
    
    def test_comment(self):
        input = "# This shouldn't affect the tokenization process\nlet x = 42"

        lex = Lex(input)
        toks = all_toks(lex)

        assert all_similar(
            toks,
            [
                TokType.LET,
                TokType.ID,
                TokType.ASSIGN,
                TokType.INT,
                TokType.EOF
            ]
        )

    def test_empty(self):
        input = ""

        lex = Lex(input)
        toks = all_toks(lex)

        assert all_similar(
            toks,
            [
                TokType.EOF
            ]
        )

    def test_interpol(self):
        input = "let greeting = \"Hello, #{\"world!  How are #{\"you\"}?\"}\""

        lex = Lex(input)
        toks = all_toks(lex)

        assert all_similar(
            toks,
            [
                TokType.LET,
                TokType.ID,
                TokType.ASSIGN,
                TokType.INTERPOL,
                TokType.INTERPOL,
                TokType.STR,
                TokType.STR,
                TokType.STR,
                TokType.EOF
            ]
        )

    def test_empty_interpol(self):
        input = "let greeting = \"Hello, #{}!\""
        
        lex = Lex(input)
        toks = all_toks(lex)

        assert all_similar(
            toks,
            [
                TokType.LET,
                TokType.ID,
                TokType.ASSIGN,
                TokType.ERR,
                TokType.EXCLAM,
                TokType.EOF
            ]
        )

    def test_unterminated_str(self):
        input = "let greeting = \"Hello, world!"
        
        lex = Lex(input)
        toks = all_toks(lex)

        assert all_similar(
            toks,
            [
                TokType.LET,
                TokType.ID,
                TokType.ASSIGN,
                TokType.ERR,
                TokType.EOF
            ]
        )
