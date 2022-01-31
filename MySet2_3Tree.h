#pragma once

#include <iostream>
#include <vector>

template<class ValueType>
class Set {
    struct Node {
        Node *parent = nullptr;
        std::vector<Node *> sons{};
        Node *left_brother = nullptr, *right_brother = nullptr;
        ValueType maximum;
        bool isBegin = false, isEnd = false;

        Node() = default;
    };

    Node *root = nullptr, *begin_ = nullptr, *end_ = nullptr;
    std::vector<Node *> node_on_level;
public:
    class iterator {
    public:
        iterator() = default;

        explicit iterator(Node *node) : node_(node) {
        }

        const ValueType &operator*() const {
            return node_->maximum;
        }

        const ValueType *operator->() const {
            return &node_->maximum;
        }

        iterator &operator++() {
            node_ = node_->right_brother;
            return *this;
        }

        iterator operator++(int) {
            auto a = *this;
            node_ = node_->right_brother;
            return a;
        }

        iterator &operator--() {
            node_ = node_->left_brother;
            return *this;
        }

        iterator operator--(int) {
            auto a = *this;
            node_ = node_->left_brother;
            return a;
        }

        bool operator==(const iterator &smth) const {
            return node_ == smth.node_;
        }

        bool operator!=(const iterator &smth) const {
            return !(*this == smth);
        }

    private:
        Node *node_{};
    };

    Set() {
        if (root != nullptr) {
            deleteAll(root);
        }
        root = new Node;
        begin_ = root;
        end_ = root;
        root->isBegin = true;
        root->isEnd = true;
    }

    template<typename Iterator>
    Set(Iterator first, Iterator last) {
        if (root != nullptr) {
            deleteAll(root);
        }
        root = new Node;
        begin_ = root;
        end_ = root;
        root->isBegin = true;
        root->isEnd = true;
        size_ = 0;
        for (Iterator i = first; i != last; ++i) {
            insertElement(*i);
        }
    }

    Set(std::initializer_list<ValueType> elements) {
        if (root != nullptr) {
            deleteAll(root);
        }
        root = new Node;
        begin_ = root;
        end_ = root;
        root->isBegin = true;
        root->isEnd = true;
        size_ = 0;
        for (auto i: elements) {
            insertElement(i);
        }
    }

    Set(const Set &set) {
        if (root != set.root) {
            if (root != nullptr) {
                deleteAll(root);
            }
            root = new Node;
            size_ = set.size_;
            node_on_level.assign(0, nullptr);
            copy_dfs(root, set.root, 0);
        }
    }

    ~Set() {
        if (root != nullptr) {
            deleteAll(root);
        }
    }

    Set &operator=(const Set &set) {
        if (root == set.root) {
            return *this;
        }
        if (root != nullptr) {
            deleteAll(root);
        }
        root = new Node;
        size_ = set.size_;
        node_on_level.assign(0, nullptr);
        copy_dfs(root, set.root, 0);
        return *this;
    }

    const size_t size() const {
        return size_;
    }

    const bool empty() const {
        return size_ == 0;
    }

    void insert(const ValueType &key) {
        insertElement(key);
    }

    void erase(const ValueType &key) {
        eraseKey(key);
    }

    iterator begin() const {
        return iterator(begin_);
    }

    iterator end() const {
        return iterator(end_);
    }

    iterator find(const ValueType &key) const {
        Node *v_find = searchValue(key);
        if (v_find->isEnd || key < v_find->maximum) {
            return iterator(end_);
        } else {
            return iterator(v_find);
        }
    }

    iterator lower_bound(const ValueType &key) const {
        Node *v_find = searchValue(key);
        if (v_find->isEnd) {
            return iterator(end_);
        } else {
            return iterator(v_find);
        }
    }

private:
    size_t size_ = 0;

    Node *searchValue(const ValueType &key) const {
        Node *current_v = root;
        while (!current_v->sons.empty()) {
            if (current_v->sons.size() < 3) {
                if (current_v->sons[0]->maximum < key) {
                    current_v = current_v->sons[1];
                } else {
                    current_v = current_v->sons[0];
                }
            } else {
                if (current_v->sons[1]->maximum < key) {
                    current_v = current_v->sons[2];
                } else if (current_v->sons[0]->maximum < key) {
                    current_v = current_v->sons[1];
                } else {
                    current_v = current_v->sons[0];
                }
            }
        }
        return current_v;
    }

    void splitParent(Node *node) {
        if (3 < node->sons.size()) {
            Node *new_parent = new Node;
            new_parent->sons = {node->sons[2], node->sons[3]};
            node->sons[2]->parent = new_parent;
            node->sons[3]->parent = new_parent;
            new_parent->parent = node->parent;
            new_parent->maximum = node->maximum;

            node->sons.pop_back();
            node->sons.pop_back();
            node->maximum = node->sons[1]->maximum;
            new_parent->right_brother = node->right_brother;
            if (node->right_brother != nullptr) {
                node->right_brother->left_brother = new_parent;
            }
            node->right_brother = new_parent;
            new_parent->left_brother = node;

            if (node == root) {
                root = new Node;
                root->sons = {node, new_parent};
                root->maximum = new_parent->maximum;
                node->parent = root;
                new_parent->parent = root;
            } else {
                for (size_t i = 0; i < node->parent->sons.size(); ++i) {
                    if (node->parent->sons[i] == node) {
                        node->parent->sons.insert(std::next(node->parent->sons.begin(), i + 1), new_parent);
                        break;
                    }
                }
                splitParent(node->parent);
            }
        }
    }

    void updateKeys(Node *node) {
        if (node == nullptr) {
            return;
        }
        if (!node->sons.empty()) {
            node->maximum = node->sons.back()->maximum;
        }
        updateKeys(node->parent);
    }

    void insertElement(const ValueType &key) {
        Node *v = searchValue(key);
        if (v->parent == nullptr) {
            ++size_;

            Node *v_end = new Node;
            Node *v_first = new Node;

            v_end->parent = root;
            v_end->left_brother = v_first;
            v_end->isEnd = true;

            v_first->parent = root;
            v_first->right_brother = v_end;
            v_first->isBegin = true;
            v_first->maximum = key;

            begin_ = v_first;
            end_ = v_end;

            root->maximum = key;
            root->isBegin = false;
            root->isEnd = false;
            root->sons.push_back(v_first);
            root->sons.push_back(v_end);
        } else {
            if (!v->isEnd && !(key < v->maximum)) {
                return;
            }
            ++size_;

            Node *new_v = new Node;
            if (v->isBegin) {
                v->isBegin = false;
                new_v->isBegin = true;
                begin_ = new_v;
            } else {
                new_v->left_brother = v->left_brother;
                v->left_brother->right_brother = new_v;
            }
            new_v->right_brother = v;
            v->left_brother = new_v;
            new_v->parent = v->parent;
            new_v->maximum = key;

            for (size_t i = 0; i < v->parent->sons.size(); ++i) {
                if (v->parent->sons[i] == v) {
                    v->parent->sons.insert(std::next(v->parent->sons.begin(), i), new_v);
                    break;
                }
            }

            updateKeys(v);
            splitParent(v->parent);
        }
        updateKeys(v);
    }

    void eraseKey(const ValueType &key) {
        Node *v = searchValue(key);
        if (v->isEnd || key < v->maximum) {
            return;
        }
        --size_;
        if (v->isBegin) {
            if (v->left_brother != nullptr) {
                v->left_brother->isBegin = true;
                begin_ = v->left_brother;
            } else {
                v->right_brother->isBegin = true;
                begin_ = v->right_brother;
            }
        }
        deleteNode(v);
    }

    void deleteNode(Node *node) {
        if (node->left_brother != nullptr) {
            node->left_brother->right_brother = node->right_brother;
        }
        if (node->right_brother != nullptr) {
            node->right_brother->left_brother = node->left_brother;
        }
        if (2 < node->parent->sons.size()) {
            for (size_t i = 0; i < node->parent->sons.size(); ++i) {
                if (node->parent->sons[i] == node) {
                    node->parent->sons.erase(node->parent->sons.begin() + i);
                    updateKeys(node);
                    break;
                }
            }
        } else {
            if (node->parent == root) {
                if (node->right_brother == nullptr) {
                    node->left_brother->parent = nullptr;
                    delete root;
                    root = node->left_brother;
                } else {
                    node->right_brother->parent = nullptr;
                    delete root;
                    root = node->right_brother;
                }
            } else {
                Node *v = node->left_brother, *v_uncle = node->parent->left_brother;
                for (size_t i = 0; i < node->parent->sons.size(); ++i) {
                    if (node->parent->sons[i] == node) {
                        if (i == 0) {
                            v = node->right_brother;
                        }
                        break;
                    }
                }
                if (node->parent->left_brother != nullptr) {
                    v->parent = v_uncle;
                    v_uncle->sons.push_back(v);
                } else {
                    v_uncle = node->parent->right_brother;
                    v->parent = v_uncle;
                    v_uncle->sons.insert(v_uncle->sons.begin(), v);
                }
                updateKeys(v);
                splitParent(v_uncle);
                deleteNode(node->parent);
            }
        }
        delete node;
    }

    void deleteAll(Node *node) {
        if (node != nullptr) {
            if (node == root) {
                node_on_level.clear();
            }
            node->parent = nullptr;
            node->left_brother = nullptr;
            node->right_brother = nullptr;
            for (size_t i = 0; i < node->sons.size(); ++i) {
                if (node->sons[i] != nullptr) {
                    deleteAll(node->sons[i]);
                }
            }
            node->sons.clear();
            delete node;
        }
    }

    void copy_dfs(Node *node1, const Node *node2, const size_t level) {
        while (node_on_level.size() < level + 2) {
            node_on_level.push_back(nullptr);
        }
        node1->left_brother = node_on_level[level];
        if (node_on_level[level] != nullptr) {
            node_on_level[level]->right_brother = node1;
        }
        node_on_level[level] = node1;

        if (node2->isBegin) {
            begin_ = node1;
            node1->isBegin = true;
        }
        if (node2->isEnd) {
            end_ = node1;
            node1->isEnd = true;
            return;
        }
        node1->maximum = node2->maximum;
        node1->sons.clear();
        for (auto &i: node2->sons) {
            Node *v = new Node;
            node1->sons.push_back(v);
            v->parent = node1;
            copy_dfs(v, i, level + 1);
        }
    }
};

