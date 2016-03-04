#include <iostream>
#include <vector>
#include <assert.h>
#include <stack>
#include <queue>
#include "bst.h"
using namespace std;

//used during building a BST
TreeNode * search_position(TreeNode* & root, int value) {
	if (!root) {
		root = new TreeNode(value);
		return root;
	}
	assert(root->val != value);
	if (root->val > value) {
		return search_position(root->left, value);
	}
	if (root->val < value) {
		return search_position(root->right, value);
	}
}	

TreeNode * build_tree(vector<int>& myvec) {
	TreeNode *root = nullptr;
	if (myvec.empty()) return root;
	for (size_t i = 0; i < myvec.size(); i++) {
		TreeNode* newNode = search_position(root, myvec[i]);
		cout << "new treenode = " << newNode->val << endl;
		inorder_traverse(root);
		cout << endl;
	}
	return root;
}




//Given root of an existed tree,
//copy this tree and return the root of the new tree 
TreeNode* copy(TreeNode* root) {
	TreeNode* node = nullptr;
	if (root) {
		node = new TreeNode(root);
		node->left = copy(root->left);
		node->right = copy(root->right);
	}
	return node;
}

void preorder_traverse(TreeNode* root) {
	if (root) {
		cout << root->val << " ";
		preorder_traverse(root->left);
		preorder_traverse(root->right);
	}
}

void inorder_traverse(TreeNode* root) {
	if (root) {
		inorder_traverse(root->left);
		cout << root->val << " ";
		inorder_traverse(root->right);
	}
}

void postorder_traverse(TreeNode* root) {
	if (root) {
		postorder_traverse(root->left);
		postorder_traverse(root->right);
		cout << root->val << " ";
	}
}

vector<vector<int>> levelorder_traverse(TreeNode* root) {
	vector<vector<int>> ans;
	if (!root) return ans;
	queue<TreeNode*> myqueue;
	TreeNode* node = nullptr;
	myqueue.push(root);
	while (!myqueue.empty()) {
		int k = myqueue.size();
		vector<int> level;
		for (int i = 0; i < k; i++) {
			node = myqueue.front();
			myqueue.pop();
			if (node->left) myqueue.push(node->left);
			if (node->right) myqueue.push(node->right);
			level.push_back(node->val);
		}
		ans.push_back(level);
	}
	return ans;
}

