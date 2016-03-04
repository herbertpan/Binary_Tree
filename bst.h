#ifndef _bst_h_
#define _bst_h_
#include <vector>
using namespace std;
//typedef class *BST TreeNode;
class TreeNode {
private:
	void destory_tree();
public:
	int val;
	TreeNode *left;
	TreeNode *right;
	explicit TreeNode(int value) : val(value) {};
	TreeNode(TreeNode* node) {
		val = node->val;
		left = nullptr;
		right = nullptr;
	}
	~TreeNode() {
		void destroy_tree();
	};
	



}; 

TreeNode* build_tree(vector<int>& myvec);
TreeNode* search_position(TreeNode*& root, int value);

void preorder_traverse(TreeNode* root);
void inorder_traverse(TreeNode* root);
void postorder_traverse(TreeNode* root);
vector<vector<int>> levelorder_traverse(TreeNode* root);


#endif