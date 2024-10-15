#include "test.hpp"

#include <iostream>

#include "../core/node.hpp"

namespace docs_gen_test {
    
    void test_node_tree() {
        docs_gen_core::node_tree t;
        t.insert(L"Player", L"Node2D");
        t.insert(L"Character", L"Node", L".");
        t.insert(L"SceneCamera", L"Camera2D", L"Character");
        t.insert(L"Interact_Handler", L"Area2D", L"Character");
        t.insert(L"CollisionShape2D", L"CollisionShape2D", L"Character/Interact_Handler");
        t.insert(L"CharacterAnimator_Hank", L"Node", L"Character");

        for (auto it = t.begin(); it != t.end(); ++it) {
            std::wcout << (*it)->name << ' ' << (*it)->path << '\n';
        }
    }

    void test_node_tree_depth() {
        docs_gen_core::node_tree t;
        t.insert(L"Player", L"Node2D");
        t.insert(L"Character", L"Node", L".");
        t.insert(L"SceneCamera", L"Camera2D", L"Character");
        t.insert(L"Interact_Handler", L"Area2D", L"Character");
        t.insert(L"CollisionShape2D", L"CollisionShape2D", L"Character/Interact_Handler");
        t.insert(L"CharacterAnimator_Hank", L"Node", L"Character");

        for (auto it = t.begin(); it != t.end(); ++it) {
            std::wcout << (*it)->name << ' ' << (*it)->path << ' ' << (*it)->depth << '\n';
        }
    }
    
} // docs_gen_test

