"""
Developer: Biren Patel
Date: 2020 April 28
Version: Python 3.7.5
Description: Regular expression tokenizer, as you would typically find in as
an option in most natural language processing tools.
"""

import re
from enum import Enum

class common_patterns(Enum):
    """
    Purpose
    -------
    A short list of common regular expression patterns for tokenization.
    
    Example
    -------
    >>> tokenizer = regex_tokenizer(common_patterns.ALPHABET.value)
    """
    
    #pattern to isolate only letters of the english alphabet
    ALPHABET = r"[a-zA-Z]+"

    #pattern matching all \w character class words
    WORDS = r"\b\w+\b"

class regex_tokenizer():
    """
    Purpose
    -------
    Natural language tokenizer which uses a compiled re regular expression to
    create tokens.

    Example
    -------
    >>> text = "His name was Robert Paulson."
    >>> tokenizer = regex_tokenizer(r"[a-zA-z]+")
    >>> tokenizer.create_tokens(text)
    """

    def __init__(self, pattern, flags=0):
        """
        Parameters
        ----------
        pattern : str
            The regular expression pattern which qualifies a valid token.
        flags : enum, optional
            The standard flags argument optionally provided to re.compile().
            The default is 0.

        Returns
        -------
        None

        """

        self.pattern = re.compile(pattern, flags)

    def create_tokens(self, corpus):
        """
        Parameters
        ----------
        corpus : str
            The body of text to be tokenized.

        Returns
        -------
        list
            List of str tokens which match self.pattern, in order of match
            occurence. See documentation for re.findall().
        """

        return self.pattern.findall(corpus)

    def create_tokens_iter(self, corpus):
        """
        Parameters
        ----------
        corpus : str
            The body of text to be tokenized.

        Returns
        -------
        iterator
            iterator of re MatchObject instances. See the documentation for
            re.finditer().
        """

        return self.pattern.finditer(corpus)