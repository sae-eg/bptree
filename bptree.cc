#include "bptree.h"
#include <vector>
#include <sys/time.h>

struct timeval
cur_time(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t;
}

void print_tree_core(NODE *n)
{
	printf("[");
	for (int i = 0; i < n->nkey; i++)
	{
		if (!n->isLeaf)
			print_tree_core(n->chi[i]);
		printf("%d", n->key[i]);
		if (i != n->nkey - 1 && n->isLeaf)
			putchar(' ');
	}
	if (!n->isLeaf)
		print_tree_core(n->chi[n->nkey]);
	printf("]");
}

void print_tree(NODE *node)
{
	print_tree_core(node);
	printf("\n");
	fflush(stdout);
}

NODE *find_leaf(NODE *node, int key)
{
	int kid;

	if (node->isLeaf)
		return node;
	for (kid = 0; kid < node->nkey; kid++)
	{
		if (key < node->key[kid])
			break;
	}

	return find_leaf(node->chi[kid], key);
}

NODE *insert_in_leaf(NODE *leaf, int key, DATA *data)
{
	int i;
	if (key < leaf->key[0])
	{
		for (i = leaf->nkey; i > 0; i--)
		{
			leaf->chi[i] = leaf->chi[i - 1];
			leaf->key[i] = leaf->key[i - 1];
		}
		leaf->key[0] = key;
		leaf->chi[0] = (NODE *)data;
	}
	else
	{
		for (i = 0; i < leaf->nkey; i++)
		{
			if (key < leaf->key[i])
				break;
		}
		for (int j = leaf->nkey; j > i; j--)
		{
			leaf->chi[j] = leaf->chi[j - 1];
			leaf->key[j] = leaf->key[j - 1];
		}

		/* CodeQuiz */
		// ↓自分でかいた
		leaf->key[i] = key;
		leaf->chi[i] = (NODE *)data;
	}
	leaf->nkey++;

	return leaf;
}

NODE *alloc_leaf(NODE *parent)
{
	NODE *node;
	if (!(node = (NODE *)calloc(1, sizeof(NODE))))
		ERR;
	node->isLeaf = true;
	node->parent = parent;
	node->nkey = 0;

	return node;
}

// TEMP作る
TEMP *alloc_temp()
{
	TEMP *temp;
	if (!(temp = (TEMP *)calloc(1, sizeof(TEMP))))
		ERR;
	temp->isLeaf = true;
	temp->nkey = 0;

	return temp;
}

NODE *insert_in_parent(NODE *new_leaf, NODE *old_leaf, int key, DATA *data)
{
	// 親ノードなければ作る
	if (old_leaf->parent == NULL)
	{
		NODE *new_root = alloc_leaf(NULL);
		new_root->isLeaf = false;
		new_root->key[0] = key;
		new_root->chi[0] = old_leaf;
		new_root->chi[1] = new_leaf;
		new_root->nkey++;

		new_leaf->parent = new_root;
		old_leaf->parent = new_root;

		Root = new_root;
		return new_root;
	}

	// 親ノードあった場合
	// else{

	// }
	return nullptr;
}

void insert(int key, DATA *data)
{
	NODE *leaf;
	// leafを見つける
	if (Root == NULL)
	{
		leaf = alloc_leaf(NULL);
		Root = leaf;
	}
	else
	{
		leaf = find_leaf(Root, key);
	}

	// leafに挿入or分割
	// リーフに空きがある場合
	if (leaf->nkey < (N - 1))
	{
		insert_in_leaf(leaf, key, data);
	}
	// 空きがない場合
	else
	{
		// split
		// future work

		NODE *new_leaf = alloc_leaf(leaf->parent);
		new_leaf->parent = leaf->parent;
		TEMP *tmp_leaf = alloc_temp();

		// tmpにcopy, insert_in_leafと同様
		int i;
		if (key < leaf->key[0])
		{
			for (i = leaf->nkey; i > 0; i--)
			{
				tmp_leaf->chi[i] = leaf->chi[i - 1];
				tmp_leaf->key[i] = leaf->key[i - 1];
			}
			tmp_leaf->key[0] = key;
			tmp_leaf->chi[0] = (NODE *)data;
		}
		else
		{
			for (i = 0; i < leaf->nkey; i++)
			{
				if (key < leaf->key[i])
					break;
			}
			for (int j = leaf->nkey; j > i; j--)
			{
				tmp_leaf->chi[j] = leaf->chi[j - 1];
				tmp_leaf->key[j] = leaf->key[j - 1];
			}
			tmp_leaf->key[i] = key;
			tmp_leaf->chi[i] = (NODE *)data;
		}
		tmp_leaf->nkey++;

		for (int i = N / 2; i < leaf->nkey; i++)
		{
			new_leaf->chi[i - (N / 2)] = tmp_leaf->chi[i];
			new_leaf->key[i - (N / 2)] = tmp_leaf->key[i];
			new_leaf->nkey++;
		}

		leaf->nkey = 2 / N; // 葉ノードのキー数減る

		insert_in_parent(new_leaf, leaf, new_leaf->key[0], data);
	}
}

void init_root(void)
{
	Root = NULL;
}

int interactive()
{
	int key;

	std::cout << "Key: ";
	std::cin >> key;

	return key;
}

int main(int argc, char *argv[])
{
	struct timeval begin, end;

	init_root();

	printf("-----Insert-----\n");
	begin = cur_time();
	while (true)
	{
		insert(interactive(), NULL);
		print_tree(Root);
	}
	end = cur_time();
	return 0;
}
