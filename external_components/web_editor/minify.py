"""Dependency-free minifier for the embedded web UI.

``web_ui.html`` is shipped inside the firmware as a C string literal, so every
byte it contains costs flash.  This module minifies the ``<style>`` and
``<script>`` blocks at codegen time, which keeps ``web_ui.html`` readable and
the single source of truth.

The JavaScript minifier is token based.  It never rewrites code: it only drops
comments and whitespace that cannot change the meaning, and it keeps every
line break that automatic semicolon insertion could depend on.  Strings,
template literals and regular expressions are scanned as opaque units so
their contents are never touched.
"""

from __future__ import annotations

import re

# --------------------------------------------------------------------------
# JavaScript
# --------------------------------------------------------------------------

# Longest-match punctuators.  Anything not listed is a single character.
_PUNCTUATORS = [
    ">>>=", "...", "===", "!==", "**=", "<<=", ">>=", ">>>", "&&=", "||=", "??=",
    "=>", "==", "!=", "<=", ">=", "&&", "||", "??", "?.", "++", "--", "+=",
    "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<", ">>", "**",
    "{", "}", "(", ")", "[", "]", ";", ",", "<", ">", "+", "-", "*", "/", "%",
    "&", "|", "^", "!", "~", "?", ":", "=", ".",
]

# Two-character sequences that would be lexed as something else if the space
# between them disappeared (including the start of a comment).
_MERGE_HAZARDS = set()
for _p in _PUNCTUATORS + ["//", "/*", "<!--"]:
    for _n in (2, 3):
        if len(_p) >= _n:
            _MERGE_HAZARDS.add(_p[:_n])

# A line break after one of these cannot end a statement: the expression
# obviously continues on the next line.
_CONTINUES_EXPRESSION = {
    "+", "-", "*", "/", "%", "**", "&&", "||", "??", "?", ":", "=", "==", "===",
    "!=", "!==", "<", ">", "<=", ">=", "<<", ">>", ">>>", "&", "|", "^", "=>",
    ",", "(", "[", "{", "+=", "-=", "*=", "/=", "%=", "**=", "&&=", "||=",
    "??=", "&=", "|=", "^=", "<<=", ">>=", ">>>=",
}

# A line break before one of these cannot start a new statement.
_CANNOT_START_STATEMENT = {")", "]", ",", ";", ".", "?."}

_WORD_START = re.compile(r"[A-Za-z_$]")
_WORD_RE = re.compile(r"[A-Za-z_$0-9]*")
_DIGITS = "0123456789"

# Words after which a "/" starts a regular expression rather than a division.
_REGEX_AFTER_WORD = {
    "return", "typeof", "instanceof", "in", "of", "new", "delete", "void",
    "case", "do", "else", "yield", "await", "throw",
}


class _Scanner:
    """Splits JavaScript into code tokens and opaque literals."""

    def __init__(self, src: str) -> None:
        self.src = src
        self.n = len(src)
        self.i = 0

    def _string(self, i: int) -> int:
        quote = self.src[i]
        j = i + 1
        while j < self.n:
            c = self.src[j]
            if c == "\\":
                j += 2
                continue
            if c == quote:
                return j + 1
            j += 1
        return self.n

    def _regex(self, i: int) -> int:
        j = i + 1
        in_class = False
        while j < self.n:
            c = self.src[j]
            if c == "\\":
                j += 2
                continue
            if c == "[":
                in_class = True
            elif c == "]":
                in_class = False
            elif c == "/" and not in_class:
                j += 1
                while j < self.n and self.src[j].isalpha():
                    j += 1
                return j
            elif c == "\n":
                return j
            j += 1
        return self.n

    def _template_expr(self, i: int) -> int:
        """Scans a ${...} substitution, returning the index past its "}"."""
        depth = 1
        prev = ""
        while i < self.n:
            c = self.src[i]
            if c == "\\":
                i += 2
                continue
            if c in "\"'":
                i = self._string(i)
                prev = "value"
                continue
            if c == "`":
                i = self._template(i)
                prev = "value"
                continue
            if c == "/" and self.src[i:i + 2] in ("//", "/*"):
                if self.src[i + 1] == "/":
                    k = self.src.find("\n", i)
                    i = self.n if k == -1 else k
                else:
                    k = self.src.find("*/", i + 2)
                    i = self.n if k == -1 else k + 2
                continue
            if c == "{":
                depth += 1
            elif c == "}":
                depth -= 1
                if depth == 0:
                    return i + 1
            prev = c
            i += 1
        return self.n

    def _template(self, i: int) -> int:
        """Scans a template literal, returning the index past its closing "`"."""
        j = i + 1
        while j < self.n:
            c = self.src[j]
            if c == "\\":
                j += 2
                continue
            if c == "`":
                return j + 1
            if c == "$" and self.src[j + 1:j + 2] == "{":
                j = self._template_expr(j + 2)
                continue
            j += 1
        return self.n

    def _number(self, i: int) -> int:
        j = i
        if self.src[j:j + 2] in ("0x", "0X", "0b", "0B", "0o", "0O"):
            j += 2
            while j < self.n and (self.src[j].isalnum() or self.src[j] == "_"):
                j += 1
            return j
        while j < self.n and self.src[j] in _DIGITS or (j < self.n and self.src[j] == "_"):
            j += 1
        if j < self.n and self.src[j] == ".":
            j += 1
            while j < self.n and (self.src[j] in _DIGITS or self.src[j] == "_"):
                j += 1
        if j < self.n and self.src[j] in "eE":
            k = j + 1
            if k < self.n and self.src[k] in "+-":
                k += 1
            if k < self.n and self.src[k] in _DIGITS:
                j = k
                while j < self.n and self.src[j] in _DIGITS:
                    j += 1
        if j < self.n and self.src[j] == "n":  # BigInt
            j += 1
        return j

    def tokens(self):
        """Yields (kind, text).  kind is ws, comment, word, num, str,
        template, regex or punct."""
        src = self.src
        prev_kind = None
        prev_text = ""
        while self.i < self.n:
            c = src[self.i]
            if c in " \t\r\n\v\f":
                j = self.i
                while j < self.n and src[j] in " \t\r\n\v\f":
                    j += 1
                yield ("ws", src[self.i:j])
                self.i = j
                continue
            if c == "/" and src[self.i:self.i + 2] == "//":
                j = src.find("\n", self.i)
                j = self.n if j == -1 else j
                yield ("comment", src[self.i:j])
                self.i = j
                continue
            if c == "/" and src[self.i:self.i + 2] == "/*":
                j = src.find("*/", self.i + 2)
                j = self.n if j == -1 else j + 2
                yield ("comment", src[self.i:j])
                self.i = j
                continue
            if c == "/" and self._regex_ok(prev_kind, prev_text):
                j = self._regex(self.i)
                yield ("regex", src[self.i:j])
                self.i = j
                prev_kind, prev_text = "regex", ""
                continue
            if c in "\"'":
                j = self._string(self.i)
                yield ("str", src[self.i:j])
                self.i = j
                prev_kind, prev_text = "str", ""
                continue
            if c == "`":
                j = self._template(self.i)
                yield ("template", src[self.i:j])
                self.i = j
                prev_kind, prev_text = "template", ""
                continue
            if c in _DIGITS or (c == "." and src[self.i + 1:self.i + 2] in _DIGITS):
                j = self._number(self.i)
                yield ("num", src[self.i:j])
                self.i = j
                prev_kind, prev_text = "num", ""
                continue
            if _WORD_START.match(c):
                j = self.i + len(_WORD_RE.match(src, self.i).group(0))
                text = src[self.i:j]
                yield ("word", text)
                self.i = j
                prev_kind, prev_text = "word", text
                continue
            for p in _PUNCTUATORS:
                if src.startswith(p, self.i):
                    yield ("punct", p)
                    self.i += len(p)
                    prev_kind, prev_text = "punct", p
                    break
            else:
                yield ("punct", c)
                self.i += 1
                prev_kind, prev_text = "punct", c

    @staticmethod
    def _regex_ok(prev_kind, prev_text) -> bool:
        if prev_kind is None:
            return True
        if prev_kind in ("str", "template", "regex", "num"):
            return False
        if prev_kind == "word":
            return prev_text in _REGEX_AFTER_WORD
        # punctuator
        if prev_text in (")", "]", "}", "++", "--"):
            return False
        return True


def _space_needed(prev, nxt) -> bool:
    pk, pt = prev
    nk, nt = nxt
    if pk in ("word", "num") and nk in ("word", "num"):
        return True
    if pk == "word" and nk in ("num", "str", "template", "regex"):
        return True
    if pk in ("str", "template", "regex") and nk in ("word", "num"):
        return True
    if pk == "num" and nt == ".":
        return True
    # Avoid accidentally forming a longer token (or a comment).
    if pt and nt and (pt[-1] + nt[0]) in _MERGE_HAZARDS:
        return True
    return False


def _newline_safe(prev, nxt) -> bool:
    pt = prev[1]
    nt = nxt[0]
    if pt in _CONTINUES_EXPRESSION:
        return True
    if nt in _CANNOT_START_STATEMENT:
        return True
    return False


def minify_js(src: str) -> str:
    """Minifies JavaScript source, preserving semantics."""
    out = []
    prev = None
    pending_newline = False
    pending_space = False

    for kind, text in _Scanner(src).tokens():
        if kind == "ws":
            if "\n" in text:
                pending_newline = True
                pending_space = False
            else:
                pending_space = True
            continue
        if kind == "comment":
            # A block comment may contain the line break that terminates the
            # statement; keep it.
            if "\n" in text:
                pending_newline = True
                pending_space = False
            continue
        token = (kind, text)
        if prev is not None:
            if pending_newline:
                sep = "" if _newline_safe(prev, token) else "\n"
            elif pending_space:
                sep = " " if _space_needed(prev, token) else ""
            else:
                sep = ""
            out.append(sep)
        out.append(text)
        prev = token
        pending_newline = False
        pending_space = False

    return "".join(out)


# --------------------------------------------------------------------------
# CSS
# --------------------------------------------------------------------------

# Whitespace next to these is never significant in the stylesheet used here.
_CSS_NO_SPACE_BEFORE = set("{;,:")
_CSS_NO_SPACE_AFTER = set("{};,:")
_CSS_COMMENT_RE = re.compile(r"/\*.*?\*/", re.S)


def minify_css(src: str) -> str:
    """Minifies a stylesheet: drops comments and redundant whitespace."""
    src = _CSS_COMMENT_RE.sub("", src)
    out = []
    prev = ""
    i = 0
    n = len(src)
    while i < n:
        c = src[i]
        if c.isspace():
            j = i
            while j < n and src[j].isspace():
                j += 1
            nxt = src[j] if j < n else ""
            if prev and nxt and prev not in _CSS_NO_SPACE_AFTER and nxt not in _CSS_NO_SPACE_BEFORE:
                out.append(" ")
            i = j
            continue
        out.append(c)
        prev = c
        i += 1
    css = "".join(out)
    # The final ";" of a declaration block is optional.
    css = re.sub(r";\}", "}", css)
    return css


# --------------------------------------------------------------------------
# HTML
# --------------------------------------------------------------------------

_HTML_COMMENT_RE = re.compile(r"<!--(?!\[if).*?-->", re.S)
_STYLE_RE = re.compile(r"(<style[^>]*>)(.*?)(</style>)", re.S | re.I)
_SCRIPT_RE = re.compile(r"(<script[^>]*>)(.*?)(</script>)", re.S | re.I)


def minify_html(src: str) -> str:
    """Minifies the embedded page: CSS, JavaScript and HTML comments."""
    src = _STYLE_RE.sub(lambda m: m.group(1) + minify_css(m.group(2)) + m.group(3), src)
    src = _SCRIPT_RE.sub(lambda m: m.group(1) + minify_js(m.group(2)) + m.group(3), src)
    src = _HTML_COMMENT_RE.sub("", src)
    # Trailing whitespace on each line costs flash and means nothing.
    src = "\n".join(line.rstrip() for line in src.split("\n"))
    return src.strip() + "\n"
