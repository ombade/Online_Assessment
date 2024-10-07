#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
using namespace std;

struct Node {
    string v;
    Node* firstChild;
    Node* nextSibling;
    Node* parent;
    int anc_locked, dec_locked, uid;
    bool isLocked;

    Node(string val, Node* p) {
        v = val;
        firstChild = nullptr;
        nextSibling = nullptr;
        parent = p;
        anc_locked = 0;
        dec_locked = 0;
        uid = 0;
        isLocked = false;
    }
};


Node* createNode(string v, Node* parent) {
    Node* newNode = new Node(v, parent);
    return newNode;
}

// Function to add a child node to the parent
void addNode(Node* parent, Node* child) {
    if (parent->firstChild == nullptr) {
        parent->firstChild = child;
    } else {
        Node* current = parent->firstChild;
        while (current->nextSibling != nullptr) {
            current = current->nextSibling;
        }
        current->nextSibling = child;
    }
}

class Tree {
private:
    Node* root;
    unordered_map<string, Node*> nodeMap;

public:
    Tree(Node* r) { root = r; }

    void fillNodeMap(Node* r) {
        if (!r) return;
        nodeMap[r->v] = r;
        fillNodeMap(r->firstChild);
        fillNodeMap(r->nextSibling);
    }

    void informDescendants(Node* r, int val) {
        Node* current = r->firstChild;
        while (current != nullptr) {
            current->anc_locked += val;
            informDescendants(current, val);
            current = current->nextSibling;
        }
    }

    bool verifyDescendants(Node* r, int id, vector<Node*>& lockedNodes) {
        if (r->isLocked) {
            if (r->uid != id) return false;
            lockedNodes.push_back(r);
        }
        if (r->dec_locked == 0) return true;

        Node* current = r->firstChild;
        bool valid = true;
        while (current != nullptr) {
            valid &= verifyDescendants(current, id, lockedNodes);
            if (!valid) return false;
            current = current->nextSibling;
        }
        return valid;
    }

    bool lock(string v, int id) {
        Node* t = nodeMap[v];
        if (t->isLocked || t->anc_locked != 0 || t->dec_locked != 0) return false;

        Node* cur = t->parent;
        while (cur) {
            cur->dec_locked++;
            cur = cur->parent;
        }

        informDescendants(t, 1);
        t->isLocked = true;
        t->uid = id;
        return true;
    }

    bool unlock(string v, int id) {
        Node* t = nodeMap[v];
        if (!t->isLocked || t->uid != id) return false;

        Node* cur = t->parent;
        while (cur) {
            cur->dec_locked--;
            cur = cur->parent;
        }

        informDescendants(t, -1);
        t->isLocked = false;
        return true;
    }

    bool upgrade(string v, int id) {
        Node* t = nodeMap[v];
        if (t->isLocked || t->anc_locked != 0 || t->dec_locked == 0) return false;

        vector<Node*> lockedNodes;
        if (verifyDescendants(t, id, lockedNodes)) {
            for (auto k : lockedNodes) {
                unlock(k->v, id);
            }
        } else {
            return false;
        }

        return lock(v, id);
    }
};


Node* buildTree(Node* root, int m, const vector<string>& nodes) {
    queue<Node*> q;
    q.push(root);
    int idx = 1;

    while (!q.empty() && idx < nodes.size()) {
        Node* current = q.front();
        q.pop();

        // Add up to m children to current node
        for (int i = 0; i < m && idx < nodes.size(); i++) {
            Node* child = createNode(nodes[idx], current);
            addNode(current, child);  // Fixing how we add child nodes
            q.push(child);
            idx++;
        }
    }

    return root;
}

int main() {
    int n, m, q;
    cin >> n >> m >> q;

    vector<string> nodes(n);
    for (int i = 0; i < n; i++) {
        cin >> nodes[i];
    }

    Node* root = createNode(nodes[0], nullptr);
    root = buildTree(root, m, nodes);

    Tree tree(root);
    tree.fillNodeMap(root);

    for (int i = 0; i < q; i++) {
        int op, uid;
        string sq;
        cin >> op >> sq >> uid;

        if (op == 1) {
            if (tree.lock(sq, uid)) {
                cout << "true\n";
            } else {
                cout << "false\n";
            }
        } else if (op == 2) {
            if (tree.unlock(sq, uid)) {
                cout << "true\n";
            } else {
                cout << "false\n";
            }
        } else if (op == 3) {
            if (tree.upgrade(sq, uid)) {
                cout << "true\n";
            } else {
                cout << "false\n";
            }
        }
    }

    return 0;
}