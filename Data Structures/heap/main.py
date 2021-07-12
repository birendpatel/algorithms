"""!
@file main.py
@author Copyright(C) 2021 Biren Patel
@brief rough prototype of a minheap data structure
"""

class Heap():
	def __init__(self, data: list = []) -> None:
		self.data = data 

		self.__make_heap()

	def __make_heap(self) -> None:
		if not self.data:
			return

		for i in range(len(self.data) // 2 - 1, -1, -1):
			self.__heapify(i)
	
	def __lchild_idx(self, i):
		idx = i * 2 + 1

		if idx >= len(self.data):
			return None

		return idx

	def __rchild_idx(self, i):
		idx = i * 2 + 2

		if idx >= len(self.data):
			return None

		return idx

	def __parent_idx(self, i):
		idx = (i - 1) // 2

		if idx < 0:
			return None

		return idx

	def __heapify(self, i) -> None:
		min = i

		l = self.__lchild_idx(i)
		r = self.__rchild_idx(i)

		if l and self.data[l] < self.data[min]:
			min = l

		if r and self.data[r] < self.data[min]:
			min = r

		if i != min:
			self.data[i], self.data[min] = \
			self.data[min], self.data[i]
		
			self.__heapify(min)
		
	def insert(self, val) -> None:
		if val is None:
			raise TypeError("cannot insert None into heap")

		self.data.append(val)

		cix = len(self.data) - 1
		pix = self.__parent_idx(cix)

		while pix is not None and self.data[cix] < self.data[pix]:
			self.data[pix], self.data[cix] = \
			self.data[cix], self.data[pix]

			cix = pix
			pix = self.__parent_idx(cix)

	def __repr__(self):
		return str(self.data)

	def __iter__(self):
		return self

	def __next__(self):
		min = self.extract()

		if min is None:
			raise StopIteration

		return min

	def extract(self):
		if not self.data:
			return None

		min = self.data[0]
		
		self.data[0] = self.data[-1]
		self.data.pop()

		self.__heapify(0)

		return min

	def sort(self) -> list:
		sorted = []

		for i in self:
			sorted.append(i)

		return sorted

	def is_heap(self) -> bool:
		if not self.data:
			return True

		return self.__is_heap(0)

	def __is_heap(self, root) -> bool:
		#base case 0: the root is a null node (child of a leaf)
		if root is None:
			return True

		l = self.__lchild_idx(root)
		r = self.__rchild_idx(root)
		
		#base case 1: leaf nodes are trivial heaps
		if not l and not r:
			return True
	
		#check if root is smaller than its immediate children.
		#this triggers early stoppage, i.e., no need to recurse
		#further into the tree
		if l and self.data[l] < self.data[root]:
			return False

		if r and self.data[r] < self.data[root]:
			return False

		#descend and check if children (if they exist) are heaps
		#may trigger base case 0
		return self.__is_heap(l) and self.__is_heap(r)
