#include "node.hpp"

#include <iostream>

namespace docs_gen_core {
    node::node(const std::wstring& name, const std::wstring& type)
        : name_(name), type_(type) {}

    node::node(const std::wstring& name, const std::wstring& type, const std::wstring& parent)
        : name_(name), type_(type), parent_(parent) {
    }

    node::node(const node& other)
        : name_(other.name_), type_(other.type_), parent_(other.parent_) {
    }

    node::node(node&& other) noexcept
        : name_(std::move(other.name_)), type_(std::move(other.type_)), parent_(std::move(other.parent_)) {
    }

    node& node::operator=(const node& other) {
        name_ = other.name_;
        type_ = other.type_;
        parent_ = other.parent_;
        return *this;
    }

    node& node::operator=(node&& other) noexcept {
        name_ = std::move(other.name_);
        type_ = std::move(other.type_);
        parent_ = std::move(other.parent_);
        return *this;
    }

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

    bool node_tree::insert(const node& node) {
        auto& parent = node.get_parent();
        if (parent.empty()) {
            if (root_ != nullptr) {
                std::cerr << "[ERROR] Root node of the scene already exists\n";
                return false;
            }

            root_ = std::make_shared<tree_node>(node.get_name(), node.get_type());
            root_->path = node.get_name();
            root_->depth = 1;
            return true;
        }
        
        if (parent == L".") {
            const auto tn = std::make_shared<tree_node>(node.get_name(), node.get_type(), root_);
            tn->path = root_->path / std::filesystem::path(node.get_name());
            tn->depth = 2;
            root_->children.push_back(tn);
            return true;
        }
        
        const auto& parent_path = root_->path / std::filesystem::path(node.get_parent()).make_preferred();
        for (auto it = begin(); it != end(); ++it) {
            if (parent_path.compare((*it)->path) == 0) {
                const auto tn = std::make_shared<tree_node>(node.get_name(), node.get_type(), *it);
                tn->path = parent_path / std::filesystem::path(node.get_name());
                tn->depth = (*it)->depth + 1;
                (*it)->children.push_back(tn);
                return true;
            }
        }
        return false;
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
