//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
#include <iostream>
#include <vector>
#include "vld.h"
#include "bst.h"

using namespace std;

int main(int argc, char * argv[]) {
	int arr[] = {10, 20, 5, 30, 15, 1, 7};
	vector<int> myvec(arr, arr + 7);
	for (int x : myvec) cout << x << " "; cout << endl;
	TreeNode *root = build_tree(myvec);
	vector<vector<int>> res = levelorder_traverse(root);
	for (auto x : res) {
		for (int y : x) {
			cout << y << " ";
		}
		cout << endl;
	}
	cout << endl;
	inorder_traverse(root); cout << endl;
	preorder_traverse(root); cout << endl;
	postorder_traverse(root); cout << endl;
	
	root->insert(17);

	vector<vector<int>> res2 = levelorder_traverse(root);
	for (auto x : res2) {
		for (int y : x) {
			cout << y << " ";
		}
		cout << endl;
	}
	cout << endl;
	cout << root->search(17) << endl;
	cout << root->search(19) << endl;
	cout << root->search(14) << endl;
	//delete_tree(root);
	root = nullptr;
	//root = new TreeNode(13);
	preorder_traverse(root); cout << endl;
	//_CrtDumpMemoryLeaks();
	getchar();
}