#ifndef DOCS_GEN_NODE_H
#define DOCS_GEN_NODE_H

#include <filesystem>
#include <memory>
#include <stack>
#include <string>
#include <vector>

namespace docs_gen_core {

    class file;
    
    class node_tree {
    public:
        class iterator;
        class const_iterator;

        struct tree_node {
            std::wstring name;
            std::wstring type;
            std::filesystem::path path;
            std::size_t depth;
            std::weak_ptr<tree_node> parent;
            std::vector<std::shared_ptr<tree_node>> children;
            std::vector<std::pair<std::wstring, std::weak_ptr<file>>> fields;

            tree_node(const std::wstring& name, const std::wstring& type);
            tree_node(const std::wstring& name, const std::wstring& type, const std::weak_ptr<tree_node>& parent);
            tree_node(const tree_node& other);
            tree_node(tree_node&& other) noexcept;
            ~tree_node() = default;

            tree_node& operator=(const tree_node& other);
            tree_node& operator=(tree_node&& other) noexcept;
        };
        
    private:
        std::shared_ptr<tree_node> root_;

    public:
        node_tree() = default;
        node_tree(const node_tree& other);
        node_tree(node_tree&& other) noexcept;
        ~node_tree() = default;

        node_tree& operator=(const node_tree& other);
        node_tree& operator=(node_tree&& other) noexcept;

        iterator begin();
        iterator end();
        
        iterator insert(const std::wstring& name, const std::wstring& type);
        iterator insert(const std::wstring& name, const std::wstring& type, const std::wstring& parent);

        // TODO implement const_iterator
        class iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::shared_ptr<tree_node>;
            using difference_type = std::ptrdiff_t;
            using pointer = std::shared_ptr<tree_node>;
            using reference = value_type&;
            using const_reference = const value_type&;

        private:
            std::stack<pointer> stack_;
            std::filesystem::path path_;
            pointer current_;

            friend class node_tree;
            
        public:
            iterator();
            explicit iterator(const std::shared_ptr<tree_node>& node);
            
            iterator& operator++();
            bool operator==(const iterator& other) const { return current_ == other.current_; }
            bool operator!=(const iterator& other) const { return current_ != other.current_; }
            reference operator*() { return current_; }
        };
    };
    
} // docs_gen_core

#endif // DOCS_GEN_NODE_H