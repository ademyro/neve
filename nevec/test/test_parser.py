import test

from nevec.parse.parse import Parse

def get_repr(input: str):
    parse = Parse(input)

    ast = parse.parse()

    return str(ast)

class TestParse:
    def test_one(self):
        input = "(1 + 2 == 3)"

        repr = get_repr(input)
        
        assert repr == input

    def test_two(self):
        input = "---not --not -not (1 + 2 + 3 < (4 > 2) * 3)"

        repr = get_repr(input)

        assert repr == input

    def test_three(self):
        input = "1 | 2 ^ 3849348 / 23 * 9 + nil / true & false"

        repr = get_repr(input)

        assert repr == input

    def test_four(self):
        input = ""
