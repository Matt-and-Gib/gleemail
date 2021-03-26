#ifndef BSTREE_H
#define BSTREE_H


template <class T>
class BinarySearchTreeNode {
public:
	BinarySearchTreeNode<T>();
	BinarySearchTreeNode<T>(T* key);

	static BinarySearchTreeNode<T>* balance(BinarySearchTreeNode<T>*);
	static BinarySearchTreeNode<T>* insert(BinarySearchTreeNode<T>*, T*);

	T* getKey() {return key;}

	void setParentNode(BinarySearchTreeNode<T>* node) {parentNode = node;}
	BinarySearchTreeNode<T>* getSmallerNode() {return smallerNode;}
	BinarySearchTreeNode<T>* getLargerNode() {return largerNode;}
private:
	T* key;
	BinarySearchTreeNode<T>* parentNode;
	BinarySearchTreeNode<T>* smallerNode;
	BinarySearchTreeNode<T>* largerNode;
};


template <class T>
class BinarySearchTree {
public:
	BinarySearchTree<T>();
	BinarySearchTree<T>(BinarySearchTreeNode<T>*);
	~BinarySearchTree<T>();

	BinarySearchTreeNode<T>* insert(BinarySearchTreeNode<T>*);
private:
	BinarySearchTreeNode<T>* rootNode;
};


#endif

/*
BSTNode* BST::Insert(BSTNode* node, int key)
{
    // If BST doesn't exist
    // create a new node as root
    // or it's reached when
    // there's no any child node
    // so we can insert a new node here
    if(node == NULL)
    {
        node = new BSTNode;
        node->Key = key;
        node->Left = NULL;
        node->Right = NULL;
        node->Parent = NULL;
    }
    // If the given key is greater than
    // node's key then go to right subtree
    else if(node->Key < key)
    {
        node->Right = Insert(node->Right, key);
        node->Right->Parent = node;
    }
    // If the given key is smaller than
    // node's key then go to left subtree
    else
    {
        node->Left = Insert(node->Left, key);
        node->Left->Parent = node;
    }
*/