#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) {
    TreeMap * map = (TreeMap *) malloc(sizeof(TreeMap));
    if (map == NULL) return NULL;
    map->lower_than = lower_than;
    map->root = NULL;
    return map;
}


void insertTreeMap(TreeMap * tree, void* key, void * value) {
    if (tree == NULL)
        return;
    
    if (searchTreeMap(tree, key) != NULL)
        return;

    TreeNode* new_node = createTreeNode(key, value);
    if (new_node == NULL)
        return;

    TreeNode* current = tree->root;
    TreeNode* parent = NULL;
    while (current != NULL) {
        parent = current;
        if (tree->lower_than(key, current->pair->key) < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    new_node->parent = parent;
    if (parent == NULL) {
        tree->root = new_node;
    } else if (tree->lower_than(key, parent->pair->key) < 0) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }
    tree->current = new_node;

}

TreeNode * minimum(TreeNode * x){
    if (x == NULL) 
        return NULL;
    while (x->left != NULL)
        x = x->left;
    return x;
}


void removeNode(TreeMap* tree, TreeNode* node) {
    if (tree == NULL || node == NULL) return;
    if (node->left == NULL && node->right == NULL) {
        if (node->parent != NULL) {
            if (node->parent->left == node) {
                node->parent->left = NULL;
            } else {
                node->parent->right = NULL;
            }
        } else {
            tree->root = NULL;
        }
        free(node);
        return;
    }
    if (node->left == NULL || node->right == NULL) {
        TreeNode* auxiliar = (node->left != NULL) ? node->left : node->right;

        if (node->parent != NULL) {
            if (node->parent->left == node) {
                node->parent->left = auxiliar;
            } else {
                node->parent->right = auxiliar;
            }

            if (auxiliar != NULL) {
                auxiliar->parent = node->parent;
            }
        } else {
            tree->root = auxiliar;
            if (auxiliar != NULL) {
                auxiliar->parent = NULL;
            }
        }
        free(node);
        return;
    }
    TreeNode* minimoDerecha = minimum(node->right);
    node->pair->key = minimoDerecha->pair->key;
    node->pair->value = minimoDerecha->pair->value;
    removeNode(tree, minimoDerecha);
}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}

Pair* searchTreeMap(TreeMap* tree, void* key) {
    if (tree == NULL || tree->root == NULL) return NULL;
    TreeNode* current = tree->root;
    while (current != NULL) {
        if (is_equal(tree, current->pair->key, key)) {
            tree->current = current;
            return current->pair;
        } else if (tree->lower_than(key, current->pair->key)) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return NULL;
}

Pair* upperBound(TreeMap* tree, void* key) {
    if (tree == NULL || tree->root == NULL) return NULL;
    TreeNode* current = tree->root;
    TreeNode* upper_bound_node = NULL;
    while (current != NULL) {
        if (tree->lower_than(current->pair->key, key)) {
            current = current->right;
        } else if (tree->lower_than(key, current->pair->key)) {
            upper_bound_node = current;
            current = current->left;
        } else {
            upper_bound_node = current;
            break;
        }
    }
    if (upper_bound_node != NULL) {
        tree->current = upper_bound_node;
        return upper_bound_node->pair;
    }
    return NULL;
}


Pair * firstTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->root == NULL)
        return NULL;

    TreeNode* min_node = minimum(tree->root);
    tree->current = min_node;
    return min_node->pair;
}

Pair * nextTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->current == NULL)
        return NULL;

    TreeNode* successor = tree->current->right;
    if (successor != NULL) {
        tree->current = minimum(successor);
        return tree->current->pair;
    } else {
        TreeNode* current = tree->current;
        TreeNode* parent = current->parent;
        while (parent != NULL && current == parent->right) {
            current = parent;
            parent = parent->parent;
        }
        tree->current = parent;
        if (parent != NULL)
            return parent->pair;
        else
            return NULL;
    }
}
