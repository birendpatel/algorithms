"""
Name: Word Vectors with PMI and SVD
Developer: Biren Patel
Description: Create word vectors using a positive pointwise mutual information
distance matrix coupled with a singular value decomposition. CPU-Friendly, a
typical book should take less than 30 seconds to train on most computers. Could
use some optimizations, i.e., using sparse data structure from the getgo, but
as-is it works.
"""

import numpy as np
from copy import deepcopy
from itertools import chain
from warnings import warn
from scipy.sparse.linalg import svds

#list of tokens converted to n-grams, avoiding zip() for readability
def n_grams(tokens, n):
    return [tokens[i:i+n] for i in range(len(tokens) - n + 1)]

#positive pointwise mutual information matrix with singular value decomposition
class ppmi_svd():
    """
    Word embeddings with singular value decomposition on positive pointwise
    mutual information distance matrix

    Custom delimiter tags prevent the context window (in create_pmi_matrix
    method) from bleeding past a well-defined section of text. For example,
    a +- 1 window on 'the ural mountains are cold delimiter french bread
    is delicious' will prevent the distance matrix from recording context
    between 'cold' and 'french'.

    If ARPACK ERROR: This is issue involving the wrapped Fortran code. Either
    your corpus is too small or some of your custom delimiter tags are too
    close. Try at least 3 words per group.

    Example
    -------
    >>> corpus = 'the ural mountains whence the ore is procured'
    >>> mdl = ppmi_svd(corpus)
    >>> mdl.train(vector_length = 5)
    >>> mdl.get_embedding('mountains')
    >>> mdl.get_similar('mountains, 3)
    """

    def __init__(self, corpus):
        """
        Parameters
        ----------
        corpus : string
            The body of text used to train the word embeddings.

        Returns
        -------
        None
        """
        self.corpus = corpus
        self.uw_len = 0
        self.word_index_dict, self.index_word_dict = {}, {}
        self.pmf = np.zeros(0, dtype='int32')
        self.ngrams = []
        self.pmi_matrix = np.zeros((0,0), dtype='float')
        self.embeddings = None
        self.DNE = np.zeros(0, dtype='float')

    def calc_univariate_pmf(self, delimiter):
        """
        Create array to estimate the probability mass function for words in the
        provided corpus.

        Parameters
        ----------
        delimiter : string
            See class docstring. The delimiter prevents the context window
            from bleeding past a well-defined section of text. This stops the
            algorithm from learning a context between disparate sections of
            text.

        Returns
        -------
        None
        """
        #get a list of the unique words in the corpus not including delimiter
        if delimiter == '':
            indv_words = self.corpus.split()
        else:
            indv_words = [i for i in self.corpus.split() if i != delimiter]

        unique_words = sorted(set(indv_words))
        self.uw_len = len(unique_words)

        #create dictionaries that map unique words to indicies, and vise-versa
        positions = [i for i in range(self.uw_len)]
        self.word_index_dict = dict(zip(unique_words, positions))
        self.index_word_dict = dict(zip(positions, unique_words))

        #create array of cardinalities
        self.pmf = np.resize(self.pmf, self.uw_len)

        for i in range(self.uw_len):
            self.pmf[i] = indv_words.count(unique_words[i])

        self.pmf = self.pmf/len(indv_words)

    def create_ngrams(self, window, delimiter):
        """
        Create ngrams from corpus

        Parameters
        ----------
        window : integer
            An odd positive integer which defines the context available on
            each side of a centered word.
        delimiter : string
            See class docstring. The delimiter prevents the context window
            from bleeding past a well-defined section of text. This stops the
            algorithm from learning a context between disparate sections of
            text.

        Returns
        -------
        None
        """
        if delimiter != '':
            #split corpus on delimiter
            sep_text = [i for i in self.corpus.split(delimiter) if len(i) > 0]

            #create n-grams within each delimited body of text
            split_sep_text = map(lambda x: x.split(), sep_text)
            ngrams_split = map(lambda x: n_grams(x, window), split_sep_text)

            #join all delimited n-gram bodies back into single object
            self.ngrams = list(chain(*ngrams_split))
        else:
            #if we don't need to delimit then just go ahead and create n-grams
            self.ngrams = n_grams(self.corpus.split(), window)

    def create_pmi_matrix(self, window):
        """
        Use n-grams and word-to-index dictionary to populate a p-pmi matrix

        Parameters
        ----------
        window : integer
            A positive integer which defines the context available on
            each side of a centered word.

        Returns
        -------
        None
        """
        #reallocate memory for the distance matrix
        self.pmi_matrix = np.resize(self.pmi_matrix, (self.uw_len, self.uw_len))

        #locate the window centering position
        center = window//2

        #used later to create synthetic probabilities p(x1,x2)
        pairs = 0

        #iterate through n-grams to populate the distance matrix
        for i in range(len(self.ngrams)):
            #make a copy of the current window and get the centered token
            curr_window = deepcopy(self.ngrams[i])
            curr_token = curr_window.pop(center)

            #get the matrix row/col index of the current token
            curr_token_id = self.word_index_dict[curr_token]

            #count up all combinations of co-occurances to the matrix
            L = map(lambda x: (self.word_index_dict[x], curr_token_id), curr_window)
            U = map(lambda x: (curr_token_id, self.word_index_dict[x]), curr_window)
            coords = list(L) + list(U)

            for j in coords:
                self.pmi_matrix[j] += 1

            #track comparisons in coords for later synthetic probabilities
            pairs += len(coords)

        #ensure diagonal entries are still equal to 0 (ignore repeated tokens)
        np.fill_diagonal(self.pmi_matrix, 0)

        #create p(x1,x2)/p(x1)p(x2) using outer product for denominator
        self.pmi_matrix = (self.pmi_matrix/pairs)/np.multiply.outer(self.pmf, self.pmf)

        #logarithm, np.where handles log(0) and performs a max(x,0) rectifier
        self.pmi_matrix = np.log2(np.where(self.pmi_matrix <= 1, 1, self.pmi_matrix))

    def compress(self, k):
        """
        Perform singular value decomposition on pmi matrix, keep unitary matrix

        Parameters
        ----------
        k : integer
            The length of word embeddings once SVD has been performed.

        Returns
        -------
        None
        """
        try:
            self.embeddings = svds(self.pmi_matrix, k)[0]
        except ValueError:
            warn('Returning PPMI matrix as the embedding matrix. '
                 '{} not less than {} unique words.'.format(k, self.uw_len))
            self.embeddings = self.pmi_matrix

        #words that do not exist at training are zero vectors in production.
        #reallocate memory for DNE attribute using k to match unitary matrix.
        self.DNE = np.zeros(k, dtype='float')

    def get_embedding(self, word):
        """
        Given a word, retrieve the embedding from the unitary matrix

        Parameters
        ----------
        word : string
            The word, in english, to embed.

        Returns
        -------
        numpy ndarray
            The k-component vector representation of the provided word. If word
            does not exist in unitary matrix, return the zero vector.
        """
        #get word vecetor using word_index_dict, if not exist return 0 vector
        try:
            return self.embeddings[self.word_index_dict[word]]
        except KeyError:
            return self.DNE

    def get_similar(self, word, k):
        """
        Given a word, retrieve the top k similar words

        Parameters
        ----------
        word : string
            The word, in english, to scan for similarities.
        k : integer
            The number of similar words to return.

        Returns
        -------
        tuple
            A tuple of strings identifying the k similar words, indexed by
            descending similarity.
        """
        word_vector = self.get_embedding(word)

        if np.sum(word_vector) == 0:
            warn('Word does not exist in training set. '
                 'Returning {} words similar to the zero vector'.format(k))

        similarity_scores = np.dot(self.embeddings, word_vector)

        top_k_indices = similarity_scores.argsort()[::-1][:k]

        top_similar = map(lambda x: self.index_word_dict[x], top_k_indices)

        return tuple(top_similar)

    def daisy_chain(self, sentence):
        """
        Given a sentence, transform to array of daisy-chained word embeddings.
        Most likely use case is to use the daisy chained array as input for
        a feedforward neural network.

        Parameters
        ----------
        sentence : string
            The sentence, in english, to daisy chain

        Returns
        -------
        numpy ndaray
            An array of daisy-chained k-component word embeddings. If the
            sentence has n words, the resulting array has n*k components.
        """
        indv_words = map(lambda x: self.get_embedding(x), sentence.split())

        return list(chain(*indv_words))

    def train(self, vector_length, window = 1, delimiter = ''):
        """
        Train PMI+SVD algorithm on the corpus provide on initialization.

        Parameters
        ----------
        vector_length : integer
            The desired number of components per trained word embedding.
        window : integer, optional
            An positive integer defining the number of context words on
            each side of a centered target word. The default is 1. The provided
            window is converted to an odd number to correctly set the
            sliding mechanism.
        delimiter : string, optional
            See class docstring. The delimiter prevents the context window
            from bleeding past a well-defined section of text. This stops the
            algorithm from learning a context between disparate sections of
            text. The default is ''.

        Raises
        ------
        ValueError
            Window must be positive.
        TypeError
            Window must be a positive integer.

        Returns
        -------
        None
        """
        if window < 1:
            raise ValueError('window must be positive')
        elif type(window) != int:
            raise TypeError('window must be a positive integer')
        else:
            #set window to correctly form n-grams at +/- a centered position
            window = 2*window + 1

        self.calc_univariate_pmf(delimiter)
        self.create_ngrams(window, delimiter)
        self.create_pmi_matrix(window)
        self.compress(vector_length)
