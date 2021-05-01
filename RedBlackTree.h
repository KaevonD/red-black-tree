#include <iostream>
#include <string>
#include <vector>

using namespace std;

template<class K, class V> class NodeT {
public:
	K key;
	V value;
	NodeT* left;
	NodeT* right;
	NodeT* parent;
	bool isBlack;
	NodeT(K val1, V val2) : key(val1), value(val2), left(nullptr), right(nullptr), parent(nullptr), isBlack(false) {};
	NodeT(K val1, V val2, bool colour) : key(val1), value(val2), left(nullptr), right(nullptr), parent(nullptr), isBlack(colour) {};
	NodeT(K val1, V val2, bool colour, NodeT* p) : key(val1), value(val2), left(nullptr), right(nullptr), parent(p), isBlack(colour) {};
};

//Template Declaration
template<class K, class V> class RedBlackTree {
public:

	RedBlackTree();
	RedBlackTree(const RedBlackTree& node);
	~RedBlackTree();
	RedBlackTree& operator=(const RedBlackTree& node);
	bool insert(K key, V val);
	bool remove(K key);
	bool search(K key);
	vector<V> search(K small, K big);
	vector<V> values();
	vector<K> keys();
	int size();
	NodeT<K, V>* getRoot();

private:

	//Helper methods and attributes here
	NodeT<K, V>* root;
	NodeT<K, V>* searchHelper(NodeT<K, V>* node, K key);
	void searchHelper2(vector<V>* result, const NodeT<K, V>* node, K small, K big);
	vector<V>* valuesHelper(vector<V>* result, NodeT<K, V>* node);
	vector<K>* keysHelper(vector<K>* result, NodeT<K, V>* node);
	int sizeHelper(NodeT<K, V>* node);
	void copyRBT(const NodeT<K, V>* node, NodeT<K, V>* copy);
	void deleteRBT(NodeT<K, V>* node);

	void rotateLeft(NodeT<K, V>* node); 
	void rotateRight(NodeT<K, V>* node); 

	//helper stuff for insert
	NodeT<K, V>* aunt(NodeT<K, V>* node);
	void swapColors(NodeT<K, V>* node1, NodeT<K, V>* node2);
	void fixRedRed(NodeT<K, V>* node);

	//helper stuff for delete
	NodeT<K, V>* nodeSearch(K key);
	NodeT<K, V>* getSmallest(NodeT<K, V>* node);
	bool isLeft(NodeT<K, V>* node);
	NodeT<K, V>* sibling(NodeT<K, V>* node);
	bool hasRedChild(NodeT<K, V>* node);
	void fixDoubleBlack(NodeT<K, V>* node);
	void swapValues(NodeT<K, V>* u, NodeT<K, V>* v);
	NodeT<K, V>* BSTreplace(NodeT<K, V>* node);
	void deleteNode(NodeT<K, V>* v);
	NodeT<K, V>* successor(NodeT<K, V>* node);
	void moveDown(NodeT<K, V>* node, NodeT<K, V>* nParent);
};


//RedBlackTree method implementations

//PRE: copy is NULL
//PARAM: node and copy to transfer tree data over
//POST: copies everything in the RBT and makes copy the root	
template<class K, class V> void RedBlackTree<K, V>::copyRBT(const NodeT<K, V>* node, NodeT<K, V>* copy) {
	if (node == nullptr) {
		return;
	}
	if (node->left != nullptr) {
		copy->left = new NodeT<K, V>(node->left->key, node->left->value, node->left->isBlack, node->parent);
		copyRBT(node->left, copy->left);
	}
	if (node->right != nullptr) {
		copy->right = new NodeT<K, V>(node->right->key, node->right->value, node->right->isBlack, node->parent);
		copyRBT(node->right, copy->right);
	}
}

//PRE: node isnt null
//PARAM: Node which everything below it is being deleted
//POST: deletes everything in the RBT below the node
template<class K, class V> void RedBlackTree<K, V>::deleteRBT(NodeT<K, V>* node) {
	if (node != nullptr) {
		deleteRBT(node->left);
		deleteRBT(node->right);
		delete node;
	}
}

template<class K, class V> RedBlackTree<K, V>::RedBlackTree() {
	root = nullptr;
}

template<class K, class V> RedBlackTree<K, V>::RedBlackTree(const RedBlackTree& node) {
	root = new NodeT<K, V>(node.root->key, node.root->value);
	copyRBT(node.root, root);
}

template<class K, class V> RedBlackTree<K, V>::~RedBlackTree() {
	deleteRBT(root);
}

template<class K, class V> RedBlackTree<K, V>& RedBlackTree<K, V>::operator=(const RedBlackTree& node) {
	if (this != &node) {
		deleteRBT();
		root = new NodeT<K, V>(node.root->key, node.root->val);
		copyTree(node.root, root);
	}
	return *this;
}

template<class K, class V> void RedBlackTree<K, V>::swapColors(NodeT<K, V>* node1, NodeT<K, V>* node2) {
	bool temp;
	temp = node1->isBlack;
	node1->isBlack = node2->isBlack;
	node2->isBlack = temp;
}

template<class K, class V> void RedBlackTree<K, V>::fixRedRed(NodeT<K, V>* node) {
	if (node == root) {
		node->isBlack = true;
		return;
	}

	NodeT<K, V>* parent = node->parent, * grandparent = parent->parent, * uncle = aunt(node);

	if (parent->isBlack != true) {
		if (uncle != nullptr && uncle->isBlack == false) {
			parent->isBlack = true;
			uncle->isBlack = true;
			grandparent->isBlack = false;
			fixRedRed(grandparent);
		}
		else {
			if (isLeft(parent)) {
				if (isLeft(node)) {
					swapColors(parent, grandparent);
				}
				else {
					rotateLeft(parent);
					swapColors(node, grandparent);
				}
				rotateRight(grandparent);
			}
			else {
				if (isLeft(node)) {
					rotateRight(parent);
					swapColors(node, grandparent);
				}
				else {
					swapColors(parent, grandparent);
				}
				rotateLeft(grandparent);
			}
		}
	}
}

template<class K, class V> NodeT<K, V>* RedBlackTree<K, V>::aunt(NodeT<K, V>* node) {
	if (node->parent == nullptr or node->parent->parent == nullptr) {
		return nullptr;
	}
	if (isLeft(node->parent)) {
		return node->parent->parent->right;
	}
	else {
		return node->parent->parent->left;
	}
}

template<class K, class V> bool RedBlackTree<K, V>::insert(K key, V val) {
	NodeT<K, V>* node = new NodeT<K, V>(key, val);
	if (root == nullptr) {
		node->isBlack = true;
		root = node;
	}
	else {
		NodeT<K, V>* temp = nodeSearch(key);

		if (temp->key == key) {
			return true;
		}

		node->parent = temp;

		if (key < temp->key) {
			temp->left = node;
		}
		else {
			temp->right = node;
		}
		fixRedRed(node);
	}
}


//PRE: node is not a nullptr
//PARAM: the node being replaced
//POST: returns the node and modifies the subtree
template<class K, class V> NodeT<K, V>* RedBlackTree<K, V>::BSTreplace(NodeT<K, V>* node) {
	if (node->left != nullptr && node->right != nullptr) {
		return successor(node->right);
	}
	if (node->left == nullptr && node->right == nullptr) {
		return nullptr;
	}
	if (node->left != nullptr) {
		return node->left;
	}
	else {
		return node->right;
	}
}

//PRE: node and nParent are not null
//PARAM: node and its new parent
//POST: chages the parent of node to nParent 
template<class K, class V> void RedBlackTree<K, V>::moveDown(NodeT<K, V>* node, NodeT<K, V>* nParent) {
	if (node->parent != nullptr) {
		if (isLeft(node)) {
			node->parent->left = nParent;
		}
		else {
			node->parent->right = nParent;
		}
	}
	nParent->parent = node->parent;
	node->parent = nParent;
}

//PRE: node isnt null
//PARAM: starting node
//POST: returns the left most node of right subtree
template<class K, class V> NodeT<K, V>* RedBlackTree<K, V>::successor(NodeT<K, V>* node) {
	NodeT<K, V>* temp = node;
	while (temp->left != nullptr) {
		temp = temp->left;
	}
	return temp;
}

//PRE: node isnt null
//PARAM: rotates node left
//POST: rotates the tree left
template<class K, class V> void RedBlackTree<K, V>::rotateLeft(NodeT<K, V>* node) {
	NodeT<K, V>* nParent = node->right;

	if (node == root) {
		root = nParent;
	}
	moveDown(node, nParent);

	node->right = nParent->left;

	if (nParent->left != nullptr) {
		nParent->left->parent = node;
	}
	nParent->left = node;
}

//PRE: node isnt null
//PARAM: rotates node right
//POST: rotates the tree right
template<class K, class V> void RedBlackTree<K, V>::rotateRight(NodeT<K, V>* node) {
	NodeT<K, V>* nParent = node->left;

	if (node == root) {
		root = nParent;
	}
	moveDown(node, nParent);

	node->left = nParent->right;

	if (nParent->right != nullptr) {
		nParent->right->parent = node;
	}

	nParent->right = node;
}

//PRE: v is not null
//PARAM: node being deleted
//POST: deletes v from tree 
template<class K, class V> void RedBlackTree<K, V>::deleteNode(NodeT<K, V>* v) {
	NodeT<K, V>* u = BSTreplace(v);
	bool uvBlack = ((u == nullptr or u->isBlack == true) && (v->isBlack == true));
	NodeT<K, V>* parent = v->parent;

	if (u == nullptr) {
		if (v == root) {
			root = nullptr;
		}
		else {
			if (uvBlack) {
				fixDoubleBlack(v);
			}
			else {
				if (sibling(v) != nullptr) {
					sibling(v)->isBlack = false;
				}
			}
			if (isLeft(v)) {
				parent->left = nullptr;
			}
			else {
				parent->right = nullptr;
			}
		}
		delete v;
		return;
	}

	if (v->left == nullptr or v->right == nullptr) {
		if (v == root) {
			v->key = u->key;
			v->value = u->value;
			v->left = v->right = nullptr;
			delete u;
		}
		else {
			if (isLeft(v)) {
				parent->left = u;
			}
			else {
				parent->right = u;
			}
			delete v;
			u->parent = parent;
			if (uvBlack) {
				fixDoubleBlack(u);
			}
			else {
				u->isBlack = true;
			}
		}
		return;
	}
	swapValues(u, v);
	deleteNode(u);
}

//PRE: u and v arnt null 
//PARAM: u and v swap values and keys
//POST: swaps the key and value of v and u with each other
template<class K, class V> void RedBlackTree<K, V>::swapValues(NodeT<K, V>* u, NodeT<K, V>* v) {
	K temp1;
	V temp2;
	temp1 = u->key;
	temp2 = u->value;
	u->key = v->key;
	u->value = v->value;
	v->key = temp1;
	v->value = temp2;
}

//PRE: node isnt null
//PARAM: the double black node
//POST: fixes the double black case of the node
template<class K, class V> void RedBlackTree<K, V>::fixDoubleBlack(NodeT<K, V>* node) {
	if (node == root) {
		return;
	}
	NodeT<K, V>* sib = sibling(node);
	NodeT<K, V>* parent = node->parent;
	if (sib == nullptr) {
		fixDoubleBlack(parent);
	}
	else {
		if (sib->isBlack == false) {
			parent->isBlack = false;
			sib->isBlack = true;
			if (isLeft(sib)) {
				rotateRight(parent);
			}
			else {
				rotateLeft(parent);
			}
			fixDoubleBlack(node);
		}
		else {
			if (hasRedChild(sib)) {
				if (sib->left != nullptr && sib->left->isBlack == false) {
					if (isLeft(sib)) {
						sib->left->isBlack = sib->isBlack;
						sib->isBlack = parent->isBlack;
						rotateRight(parent);
					}
					else {
						sib->left->isBlack = parent->isBlack;
						rotateRight(sib);
						rotateLeft(parent);
					}
				}
				else {
					if (isLeft(sib)) {
						sib->right->isBlack = parent->isBlack;
						rotateLeft(sib);
						rotateRight(parent);
					}
					else {
						sib->right->isBlack = sib->isBlack;
						sib->isBlack = parent->isBlack;
						rotateLeft(parent);
					}
				}
				parent->isBlack = true;
			}
			else {
				sib->isBlack = false;
				if (parent->isBlack == true) {
					fixDoubleBlack(parent);
				}
				else {
					parent->isBlack = true;
				}
			}
		}
	}
}

//PRE: node is null
//PARAM: node being checked
//POST: returns true if nd has a red child otherwise false
template<class K, class V> bool RedBlackTree<K, V>::hasRedChild(NodeT<K, V>* node) {
	if ((node->left != nullptr && node->left->isBlack == false) || (node->right != nullptr && node->right->isBlack == false)) {
		return true;
	}
	return false;
}

//PRE: node isnt null
//PARAM: node whos sibling we are finding
//POST: returns the sibling of node if it exists
template<class K, class V> NodeT<K, V>* RedBlackTree<K, V>::sibling(NodeT<K, V>* node) {
	if (node->parent == nullptr) {
		return nullptr;
	}
	if (isLeft(node)) {
		return node->parent->right;
	}
	return node->parent->left;
}

//PRE: node isnt null
//PARAM: node being checked
//POST: returns true if node is the left child
template<class K, class V> bool RedBlackTree<K, V>::isLeft(NodeT<K, V>* node) {
	if (node == node->parent->left) {
		return true;
	}
	return false;
}

//PRE: node isnt null
//PARAM: base node
//POST: returns node pointer to smallest value in subtree
template<class K, class V> NodeT<K, V>* RedBlackTree<K, V>::getSmallest(NodeT<K, V>* node) {
	NodeT<K, V>* current = node;
	while (current->left != nullptr) {
		current = current->left;
	}
	return current;
}

//PRE: key is valid
//PARAM: key being searched for
//POST: returns the node which has the given key 
template<class K, class V> NodeT<K, V>* RedBlackTree<K, V>::nodeSearch(K key) {
	NodeT<K, V>* current = root;
	while (current != nullptr) {
		if (key < current->key) {
			if (current->left == nullptr) {
				break;
			}
			else {
				current = current->left;
			}
		}
		else if (key == current->key) {
			break;
		}
		else {
			if (current->right == nullptr) {
				break;
			}
			else {
				current = current->right;
			}
		}
	}
	return current;
}
template<class K, class V> bool RedBlackTree<K, V>::remove(K key) {
	if (root == nullptr) {
		return false;
	}
	NodeT<K, V>* x = nodeSearch(key); 
	if (x->key != key) {
		return false;
	}
	deleteNode(x);
	return true;
}

template<class K, class V> NodeT<K,V>* RedBlackTree<K, V>::searchHelper(NodeT<K, V>* node, K key) {
	if (node == NULL || node->key == key) {
		return node;
	}

	if (node->key < key) {
		return searchHelper(node->right, key);
	}

	return searchHelper(node->left, key);
}

template<class K, class V> bool RedBlackTree<K, V>::search(K key) {
	NodeT<K, V>* temp = root;
	if (searchHelper(temp, key) != NULL) {
		return true;
	}
	return false;
}

//PRE: small value in parameter is less than big, node isnt null
//PARAM: vector that has the keys, the current node, smallest value, and biggest value
//POST: returns all values between small and big 
template<class K, class V> void RedBlackTree<K, V>::searchHelper2(vector<V>* result, const NodeT<K, V>* node, K small, K big) {
	if (node == nullptr) {
		return;
	}
	if (node->key >= small) {
		searchHelper2(result, node->left, small, big);
	}
	result->push_back(node->value);
	if (node->key <= big) {
		searchHelper2(result, node->right, small, big);
	}
}

template<class K, class V> vector<V> RedBlackTree<K, V>::search(K small, K big) {
	vector<V> result;
	searchHelper2(&result, root, small, big);
	return result;
}

//PRE: node isnt null
//PARAM: vector of the results
//POST: returns a vector containing all the values in the tree in ascending key order
template<class K, class V> vector<V>* RedBlackTree<K, V>::valuesHelper(vector<V>* result, NodeT<K, V>* node) {
	if (node == nullptr) {
		return result;
	}
	valuesHelper(result, node->left);

	result->push_back(node->value);

	valuesHelper(result, node->right);
}

template<class K, class V> vector<V> RedBlackTree<K, V>::values() {
	vector<V>* result = new vector<V>;

	return *valuesHelper(result, root);
}

template<class K, class V> vector<K>* RedBlackTree<K, V>::keysHelper(vector<K>* result, NodeT<K, V>* node) {
	if (node == nullptr) {
		return result;
	}

	keysHelper(result, node->left);

	result->push_back(node->key);

	keysHelper(result, node->right);
}

template<class K, class V> vector<K> RedBlackTree<K, V>::keys() {
	vector<K>* result = new vector<K>;

	return *keysHelper(result, root);
}

//PRE: node isnt null
//PARAM: current node
//POST: returns size of tree
template<class K, class V> int RedBlackTree<K, V>::sizeHelper(NodeT<K, V>* node) {
	int count = 0;
	if (node->left != nullptr) {
		count += sizeHelper(node->left);
		count++;
	}
	if (node->right != nullptr) {
		count += sizeHelper(node->right);
		count++;
	}
	return count;
}

template<class K, class V> int RedBlackTree<K, V>::size() {
	NodeT<K, V>* temp = root;
	if (root == nullptr) {
		return 0;
	}
	return sizeHelper(root)+1;
}

template<class K, class V> NodeT<K, V>* RedBlackTree<K, V>::getRoot() {
	return root;
}