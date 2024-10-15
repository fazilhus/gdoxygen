#include "node.hpp"

#include <iostream>

namespace docs_gen_core {

    node_tree::tree_node::tree_node(const std::wstring& name, const std::wstring& type)
        : name(name), type(type), depth(0), parent({}) {
    }

    node_tree::tree_node::tree_node(const std::wstring& name, const std::wstring& type,
        const std::weak_ptr<tree_node>& parent)
        : name(name), type(type), depth(0), parent(parent) {
    }

    node_tree::tree_node::tree_node(const tree_node& other)
        : name(other.name), type(other.type), path(other.path), depth(other.depth),
        parent(other.parent), children(other.children) {
    }

    node_tree::tree_node::tree_node(tree_node&& other) noexcept
        : name(std::move(other.name)), type(std::move(other.type)), path(std::move(other.path)),
        depth(other.depth), parent(std::move(other.parent)), children(std::move(other.children)) {
     }

    node_tree::tree_node& node_tree::tree_node::operator=(const tree_node& other) {
        name = other.name;
        type = other.type;
        path = other.path;
        depth = other.depth;
        parent = other.parent;
        children = other.children;
        return *this;
    }

    node_tree::tree_node& node_tree::tree_node::operator=(tree_node&& other) noexcept {
        name = std::move(other.name);
        type = std::move(other.type);
        path = std::move(other.path);
        depth = other.depth;
        parent = std::move(other.parent);
        children = std::move(other.children);
        return *this;
    }

    node_tree::node_tree(const node_tree& other)
        : root_(other.root_) {
    }

    node_tree::node_tree(node_tree&& other) noexcept
        : root_(std::move(other.root_)) {
    }

    node_tree& node_tree::operator=(const node_tree& other) {
        root_ = other.root_;
        return *this;
    }

    node_tree& node_tree::operator=(node_tree&& other) noexcept {
        root_ = std::move(other.root_);
        return *this;
    }

    node_tree::iterator node_tree::begin() {
        return iterator(root_);
    }
    
    node_tree::iterator node_tree::end() {
        return {};
    }

    node_tree::iterator node_tree::insert(const std::wstring& name, const std::wstring& type) {
        return insert(name, type, {});
    }

    node_tree::iterator node_tree::insert(const std::wstring& name, const std::wstring& type, const std::wstring& parent) {
        if (parent.empty()) {
            if (root_ != nullptr) {
                std::cerr << "[ERROR] Root node of the scene already exists\n";
                return end();
            }

            root_ = std::make_shared<tree_node>(name, type);
            root_->path = name;
            root_->depth = 1;
            return iterator(root_);
        }
        
        if (parent == L".") {
            const auto tn = std::make_shared<tree_node>(name, type, root_);
            tn->path = root_->path / std::filesystem::path(name);
            tn->depth = 2;
            root_->children.push_back(tn);
            return iterator(tn);
        }
        
        const auto& parent_path = root_->path / std::filesystem::path(parent).make_preferred();
        for (auto it = begin(); it != end(); ++it) {
            if (parent_path.compare((*it)->path) == 0) {
                const auto tn = std::make_shared<tree_node>(name, type, *it);
                tn->path = parent_path / std::filesystem::path(name);
                tn->depth = (*it)->depth + 1;
                (*it)->children.push_back(tn);
                return iterator(tn);
            }
        }
        return end();
    }

    node_tree::iterator::iterator()
        : path_(""), current_(nullptr) {}

    node_tree::iterator::iterator(const std::shared_ptr<tree_node>& node)
        : path_(""), current_(node) {
        for (auto it = current_->children.rbegin(); it != current_->children.rend(); ++it) {
            stack_.push(*it);
        }
    }

    node_tree::iterator& node_tree::iterator::operator++() {
        if (stack_.empty()) {
            current_ = nullptr;
            return *this;
        }

        auto top = stack_.top();
        stack_.pop();
        if (!top->children.empty()) {
            for (auto it = top->children.rbegin(); it != top->children.rend(); ++it) {
                stack_.push(*it);
            }
        }
        current_ = top;
        return *this;
    }
    
} // docs_gen_core
