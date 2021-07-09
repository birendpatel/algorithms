/*
* @file main.c
* @author Copyright (C) 2021 Biren Patel, MIT Licensed
* @brief This is a quick and rough sketch of a binary search tree complete
* with insert, delete, in-order, level-order, min, max, priorty queue,
* rebalance, and successor operations. See main() for a quick demonstration.
* This was written over the course of a few hours in a forward pass so there
* are bound to be errors. Do not use this for anything but educational purposes.
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
* @enum QueueError
* @brief Queue API error codes
*******************************************************************************/
typedef enum QueueError {
	QUEUE_ESUCCESS = 0,
	QUEUE_ENOMEM,
	QUEUE_EINVALID,
	QUEUE_EFULL,
	QUEUE_EEMPTY,
	QUEUE_ENULLPTR,
} QueueError;

/*******************************************************************************
* @struct queue
* @brief FIFO queue required for non-recursive breadth first search
* @details This queue stores raw memory addresses
*******************************************************************************/
typedef struct Queue {
	int front;
	int back;
	int len;
	int cap;
	void *buf[];
} Queue;

/*******************************************************************************
* @fn queue_init
* @initialize queue
******************************************************************************/
QueueError queue_init(Queue **self, const int n)
{
	if (!n) {
		return QUEUE_EINVALID;
	}

	*self = malloc(sizeof(Queue) + (sizeof(void*) * n));

	(*self)->front = 0;
	(*self)->back = 0;
	(*self)->len = 0;
	(*self)->cap = n;

	return QUEUE_ESUCCESS;
}

/*******************************************************************************
* @fn queue_enqueue
* @brief push data to back of queue
*******************************************************************************/
QueueError queue_enqueue(Queue *self, void *data)
{
	if (!self || !data) {
		return QUEUE_ENULLPTR;
	}

	if (self->len == self->cap) {
		return QUEUE_EFULL;
	}

	self->buf[self->back] = data;
	self->back = (self->back + 1) % self->cap;
	self->len++;
	return QUEUE_ESUCCESS;
}

/*******************************************************************************
* @fn queue_dequeue
* @brief pop data from front queue
*******************************************************************************/
QueueError queue_dequeue(Queue *self, void **data)
{
	if (!self || !data) {
		return QUEUE_ENULLPTR;
	}

	if (self->len == 0) {
		return QUEUE_EEMPTY;
	}

	memcpy(data, &self->buf[self->front], sizeof(void*));
	self->front = (self->front + 1) % self->cap;
	self->len--;

	return QUEUE_ESUCCESS;
}

//queue helpers
#define queue_is_empty(q) (q->len == 0)

/*******************************************************************************
* @enum TreeError
* @brief Tree API error codes
*******************************************************************************/
typedef enum TreeError {
	TREE_ESUCCESS = 0,
	TREE_ENOMEM,
	TREE_ENULLPTR,
	TREE_EEMPTY,
	TREE_EUNDEFINED,
} TreeError;

/*******************************************************************************
* @struct TreeNode
* @brief Binary search tree nodes
*******************************************************************************/
typedef struct TreeNode{
	struct TreeNode *left;
	struct TreeNode *right;
	struct TreeNode *parent;
	int value;
} TreeNode;

/******************************************************************************
* @struct Tree
* @brief Binary search tree metadata
******************************************************************************/
typedef struct Tree{
	TreeNode *root;
	int n;
} Tree;

//prototypes
TreeError tree_init(Tree **self);
void treenode_free(TreeNode *const self);
void tree_free(Tree *const self);
TreeError tree_insert(Tree *const self, const int value);
TreeNode *find_parent_node(Tree *const self, const int value);
TreeError tree_dfs(Tree *self, int **data, int *n);
int  __tree_dfs(TreeNode *self, int *data, int idx);
TreeNode *make_child_node(const int value);
TreeError tree_max(Tree *self, int *max);
TreeError tree_min(Tree *self, int *min);
TreeNode *__tree_min(TreeNode *self);
TreeError tree_bfs(Tree *self, int **data, int *n);
int __tree_bfs(Tree *self, int *data, Queue *tasks);
TreeError tree_balance(Tree *self);
TreeNode *
__tree_balance(TreeNode *parent, TreeNode **data, const int lo, const int hi);
TreeError dfs_nodes(Tree *self, TreeNode ***data);
int __dfs_nodes(TreeNode *self, TreeNode **data, int idx);
TreeError tree_delete(Tree *self, TreeNode *node);
void replace(Tree *const self, TreeNode *const parent, TreeNode *const child);
TreeNode *treenode_successor(TreeNode *self);
TreeError tree_min_extract(Tree *self, int *min);

/******************************************************************************
* @fn tree_init
* @brief Initialize an empty binary search tree
******************************************************************************/
TreeError tree_init(Tree **self)
{
	*self = malloc(sizeof(Tree));

	if (!*self) {
		return TREE_ENOMEM;
	}

	(*self)->root = NULL;
	(*self)->n = 0;

	return TREE_ESUCCESS;
}

/*******************************************************************************
* @fn tree_free
* @brief Deallocate a binary search tree and all of its nodes
*******************************************************************************/
void tree_free(Tree *const self)
{
	treenode_free(self->root);

	free(self);
}

/******************************************************************************
* @fn treenode_free
* @brief Deallocate tree nodes in depth first post order
******************************************************************************/
void treenode_free(TreeNode *const self)
{
	if (self->left) {
		treenode_free(self->left);
	}

	if (self->right) {
		treenode_free(self->right);
	}

	free(self);
}

/******************************************************************************
* @fn tree_insert
* @brief Insert a node with the given value into the binary search tree
******************************************************************************/
TreeError tree_insert(Tree *const self, const int value)
{
	if (!self) {
		return TREE_ENULLPTR;
	}

	TreeNode *parent = find_parent_node(self, value);
	TreeNode *child = make_child_node(value);

	if (!child) {
		return TREE_ENOMEM;
	}

	if (!parent) {
		self->root = child;
	} else {
		if (parent->value > value) {
			parent->left = child;
		} else {
			parent->right = child;
		}
	}

	child->parent = parent;

	self->n += 1;

	return TREE_ESUCCESS;
}

/******************************************************************************
* @fn make_child_node
* @brief allocate child node with undefined parent
******************************************************************************/
TreeNode *make_child_node(const int value)
{
	TreeNode *new = malloc(sizeof(TreeNode));

	if (!new) {
		return NULL;
	}

	new->left = NULL;
	new->right = NULL;
	new->parent = NULL;
	new->value = value;

	return new;
}

/******************************************************************************
* @fn find_parent_node
* @brief determine who should be the parent, if any, of the given value
******************************************************************************/
TreeNode *find_parent_node(Tree *const self, const int value)
{
	TreeNode *prev = NULL;
	TreeNode *curr = self->root;

	while (curr) {
		prev = curr;

		if (curr->value >=  value) {
			curr = curr->left;
		} else {
			curr = curr->right;
		}
	}

	return prev;
}

/******************************************************************************
* @fn tree_dfs
* @brief Traverse binary search tree depth first in order
* @return Array of in-order tree nodes
******************************************************************************/
TreeError tree_dfs(Tree *self, int **data, int *n)
{
	*n = self->n;

	if (!self) {
		return TREE_ENULLPTR;
	}

	if (!self->root) {
		return TREE_EEMPTY;
	}

	*data = malloc(sizeof(int) * self->n);

	if (!*data) {
		return TREE_ENOMEM;
	}

	int checksum = __tree_dfs(self->root, *data, 0);

	assert(checksum == self->n);

	return TREE_ESUCCESS;
}

/******************************************************************************
* @fn __tree_dfs
* @brief recursive insertion of nodes into array
******************************************************************************/
int  __tree_dfs(TreeNode *self, int *data, int idx)
{
	if (self->left) {
		idx = __tree_dfs(self->left, data, idx);
	}

	data[idx++] = self->value;

	if (self->right) {
		idx = __tree_dfs(self->right, data, idx);
	}
	
	return idx;
}

/******************************************************************************
* @fn tree_max
* @brief fetch maximum value present in the binary search tree
******************************************************************************/
TreeError tree_max(Tree *self, int *max)
{
	if (!self || !max) {
		return TREE_ENULLPTR;
	}

	if (!self->root) {
		return TREE_EEMPTY;
	}

	TreeNode *curr = self->root;

	while (curr->right) {
		curr = curr->right;
	}

	*max = curr->value;

	return TREE_ESUCCESS;
}

/******************************************************************************
* @fn tree_min
* @brief fetch minimum value present in the binary search tree
* @details this one is recursive just for fun
******************************************************************************/
TreeError tree_min(Tree *self, int *min)
{
	if (!self || !min) {
		return TREE_ENULLPTR;
	}

	if (!self->root) {
		return TREE_EEMPTY;
	}

	*min = __tree_min(self->root)->value;

	return TREE_ESUCCESS;
}

/******************************************************************************
* @fn __tree_min
* @brief recursive implementation of tree min descent
* @return the node itself, not its contained value
******************************************************************************/
TreeNode *__tree_min(TreeNode *self)
{
	if (!self->left) {
		return self;
	}

	return __tree_min(self->left);
}

/*******************************************************************************
* @fn tree_bfs
* @brief breadth first search, place data into heap array
*******************************************************************************/
TreeError tree_bfs(Tree *self, int **data, int *n)
{
	*n = self->n;

	if (!self) {
		return TREE_ENULLPTR;
	}

	if (!self->root) {
		return TREE_EEMPTY;
	}

	*data = malloc(sizeof(int) * self->n);

	if (!data) {
		return TREE_ENULLPTR;
	}

	Queue *q = NULL;
	QueueError err = QUEUE_ESUCCESS;

	err = queue_init(&q, (self->n + 2 + 1) / 2);

	if (err) {
		return TREE_ENOMEM;
	}

	int checksum = __tree_bfs(self, *data, q);

	assert(checksum == self->n);

	return TREE_ESUCCESS;
}

/*******************************************************************************
* @fn __tree_bfs
* @brief bfs implementation (caller handles errors and setup as BFS is messy)
* @return checksum of total elements added to data array
*******************************************************************************/
int __tree_bfs(Tree *self, int *data, Queue *tasks)
{
	TreeNode *curr = NULL;
	int idx = 0;

	queue_enqueue(tasks, self->root);

	while (!queue_is_empty(tasks)) {
		queue_dequeue(tasks, (void**) &curr);

		data[idx++] = curr->value;
		
		if (curr->left) {
			queue_enqueue(tasks, curr->left);
		}

		if (curr->right) {
			queue_enqueue(tasks, curr->right);
		}
	}

	return idx;
}

/*******************************************************************************
* @fn tree_balance
* @brief balance a binary search tree
* @details This is one of the typical O(n) rebalancing algorithms. First, we
* place references to all the BST nodes into an array via DFS in-order traversal
* with dfs_nodes(). Then in __tree_balance() we recursively splice the array
* into halves and restructure the node child and parent pointers using the
* reference at the middle of the array.
*******************************************************************************/
TreeError tree_balance(Tree *self)
{
	if (!self) {
		return TREE_ENULLPTR;
	}

	if (!self->root) {
		return TREE_EEMPTY;
	}

	TreeNode **sorted_nodes = NULL; /* an array of pointers to TreeNode */
	TreeError err = TREE_ESUCCESS;

	err = dfs_nodes(self, &sorted_nodes);

	if (err) {
		return TREE_ENOMEM;
	}
	
	self->root = __tree_balance(NULL, sorted_nodes, 0, self->n - 1);
	
	free(sorted_nodes);

	return TREE_ESUCCESS;
}

/*******************************************************************************
* @fn __tree_balance
* @brief recursively rebalance a binary search tree given sorted in-order nodes
*******************************************************************************/
TreeNode *
__tree_balance(TreeNode *parent, TreeNode **data, const int lo, const int hi)
{
	if (lo > hi) {
		return NULL;
	}

	int mid = (lo + hi) /  2;

	TreeNode *root = data[mid];

	root->left = __tree_balance(root, data, lo, mid - 1);

	root->right = __tree_balance(root, data, mid + 1, hi);

	root->parent = parent;

	return root;
}

/*******************************************************************************
* @fn dfs_nodes
* @brief tree_balance() helper, return array of tree nodes as dfs in-order
* @details oh no! A triple pointer appears! To ensure that the rebalancing
* algorithm has O(n) complexity and tight memory allocations, the returned
* array does not actually contain TreeNode structs themselves, but rather
* references. This cuts the memory requirements down to 25%. 
*******************************************************************************/
TreeError dfs_nodes(Tree *self, TreeNode ***data)
{
	*data = malloc(sizeof(void*) * self->n);

	if (!*data) {
		return TREE_ENOMEM;
	}

	int checksum = __dfs_nodes(self->root, *data, 0);
	assert(checksum == self->n);

	return TREE_ESUCCESS;
}

/*******************************************************************************
* @fn __dfs_nodes
* @brief recursive dfs in-order, like __tree_dfs but for nodes not values
*******************************************************************************/
int __dfs_nodes(TreeNode *self, TreeNode **data, int idx)
{
	if (self->left) {
		idx = __dfs_nodes(self->left, data, idx);
	}

	data[idx++] = self;

	if (self->right) {
		idx = __dfs_nodes(self->right, data, idx);
	}

	return idx;
}

/*******************************************************************************
* @fn tree_delete
* @brief delete a given node from the BST
*******************************************************************************/
TreeError tree_delete(Tree *self, TreeNode *node)
{
	if (!self || !node) {
		return TREE_ENULLPTR;
	}

	if (!node->left) {
		//node has either no children or a single right child
		replace(self, node, node->right);
	} else if (!node->right) {
		//node has a single left child
		replace(self, node, node->right);
	} else {
		//node has two children, find the replacement node
		TreeNode *successor = treenode_successor(node->right);

		if (node->right != successor) {
			//the successor is not the node's immediate child
			//first the successor must be replaced with its
			//own right child. Then the successor must update
			//its new right child to the input nodes right child
			replace(self, successor, successor->right);
			successor->right = node->right;
			successor->right->parent = successor;
		}

		replace(self, node, successor);
		successor->left = node->left;
		successor->left->parent = successor;
	}

	free(node);
	self->n -= 1;

	return TREE_ESUCCESS;
}

/*******************************************************************************
* @fn replace
* @brief replace parent with child but don't update child L/R pointers
*******************************************************************************/
void replace(Tree *const self, TreeNode *const parent, TreeNode *const child)
{
	TreeNode *const grandparent = parent->parent;

	//update grandparent's child pointer
	if (!grandparent) {
		self->root = child;
	} else if (parent == grandparent->left) {
		grandparent->left = child;
	} else {
		grandparent->right = child;
	}

	//update child's parent pointer
	if (child) {
		child->parent = grandparent;
	}

	return;
}

/*******************************************************************************
* @fn treenode_successor
* @brief finds the successor node to the given node where the successor is
* defined as the node with the min value in the set of all nodes with values
* greater than the given node.
*******************************************************************************/
TreeNode *treenode_successor(TreeNode *self)
{
	TreeNode *prev = self->parent;
	TreeNode *curr = self;

	if (curr->right) {
		//right subtree exists so successor is a child
		curr = curr->right;

		while (curr->left) {
			curr = curr->left;
		}
	} else {
		//successor is a parent
		while (prev && prev->right == curr) {
			curr = prev;
			prev = prev->parent;
		}
	}

	return curr;
}

/*******************************************************************************
* @fn tree_min_extract
* @brief get min value in BST and delete the containing node
*******************************************************************************/
TreeError tree_min_extract(Tree *self, int *min)
{
	if (!self || !min) {
		return TREE_ENULLPTR;
	}

	TreeNode *min_node = __tree_min(self->root);
	
	assert(min_node);

	*min = min_node->value;

	tree_delete(self, min_node);

	return TREE_ESUCCESS;
}

/*
* Demonstration of BST usage, without error checking.
*/

#include <stdio.h>
int main(void)
{
	//initialize BST
	Tree *x = NULL;
	tree_init(&x);

	//insert data in a manner which reduces the BST to a linked list
	tree_insert(x, 5);
	tree_insert(x, 10);
	tree_insert(x, 15);
	tree_insert(x, 20);
	tree_insert(x, 25);
	tree_insert(x, 30);
	tree_insert(x, 35);

	//depth first in order traversal
	int *data = NULL;
	int n = 0;

	tree_dfs(x, &data, &n);

	for (int i = 0; i < n; i ++) {
		printf("%d ", data[i]);
	}
	
	puts("");
	
	//breadth first level order traversal
	tree_bfs(x, &data, &n);

	for (int i = 0; i < n; i++) {
		printf("%d ", data[i]);
	}

	puts("");

	//fetch the minimum and maximum values
	int max = 0;
	int min = 0;

	tree_max(x, &max);
	tree_min(x, &min);

	printf("max: %d\nmin: %d\n", max, min);
	
	//rebalance the BST so that it has a height of log(n)
	tree_balance(x);
	
	tree_bfs(x, &data, &n);

	for (int i = 0; i < n; i++) {
		printf("%d ", data[i]);
	}

	puts("");
	
	//operate the BST as a minimum priority queue
	for (int i = 0; i < 3; i++) {
		tree_min_extract(x, &min);
		printf("%d\n", min);
	}

	return EXIT_SUCCESS;
}
