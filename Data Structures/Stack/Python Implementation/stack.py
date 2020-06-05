"""
Author: Biren Patel
Description: Stack abstract data type.
"""

class Stack():
    def __init__(self, dtype):
        """
        purpose: a homogenous and unbounded stack.
        @ dtype : the data type to be stored in the stack.
        """
        self.dtype = dtype
        self.data = []

    def push(self, item):
        """
        purpose: push an item onto the stack.
        @ item : must be of type specificied on initialization
        """
        if type(item) is self.dtype:
            self.data.append(item)
        else:
            raise TypeError("pushed item is not {}".format(self.dtype))

    def pop(self):
        """
        purpose: pop the most recently pushed item off the stack.
        """
        if self.is_empty():
            raise IndexError("pop attempted from empty stack")
        else:
            return self.data.pop()

    def peek(self):
        """
        purpose: examine, but do not remove, the most recently pushed item.
        """
        if self.is_empty():
            raise IndexError("stack is empty")
        else:
            return self.data[-1]

    def is_empty(self):
        """
        purpose: determine if the stack is currently empty.
        """
        return not self.data

    def clear(self):
        """
        purpose: empty all contents of the stack.
        """
        self.data = []

    def print_stack(self):
        """
        purpose: display current contents of stack to stdin.
        """
        print(' '.join([str(i) for i in self.data]))
