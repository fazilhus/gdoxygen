#ifndef DOCS_GEN_NODE_H
#define DOCS_GEN_NODE_H

#include <filesystem>
#include <memory>
#include <stack>
#include <string>
#include <vector>

namespace docs_gen_core {

    class node {
        std::wstring name_;
        std::wstring type_;
        std::wstring parent_;

    public:
        node(const std::wstring& name, const std::wstring& type);
        node(const std::wstring& name, const std::wstring& type, const std::wstring& parent);
        node(const node& other);
        node(node&& other) noexcept;
        ~node() = default;

        node& operator=(const node& other);
        node& operator=(node&& other) noexcept;

        [[nodiscard]] const std::wstring& get_name() const { return name_; }
        [[nodiscard]] const std::wstring& get_type() const { return type_; }
        [[nodiscard]] const std::wstring& get_parent() const { return parent_; }
    };

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
        
        bool insert(const node& node);

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