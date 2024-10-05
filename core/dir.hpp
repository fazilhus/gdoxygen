#ifndef DOCS_GEN_DIR_H
#define DOCS_GEN_DIR_H

#include <filesystem>
#include <vector>

#include <memory>
#include <unordered_map>

#include "file.hpp"

namespace docs_gen_core {

	class dir {
		std::filesystem::path path_;
		std::vector<std::wstring> ignored_folders_;

		std::unordered_map<std::wstring, std::shared_ptr<scene_file>> file_tree_;
		std::unordered_map<std::wstring, std::shared_ptr<script_file>> script_files_;
		std::unordered_map<std::wstring, std::shared_ptr<resource_file>> resource_files_;

	public:
		dir() = default;
		dir(const dir& other) = delete;
		dir(dir&& other) = delete;
		~dir() = default;

		dir& operator=(const dir& other) = delete;
		dir& operator=(dir&& other) = delete;

		[[nodiscard]] bool set_path(const std::wstring& path);
		void set_ignored_folders(const std::vector<std::wstring>& folders) { ignored_folders_ = folders; }
		void push_ignored_folder(const std::wstring& folder) { ignored_folders_.push_back(folder); }

		void construct_file_tree();
		void gen_docs();

	private:
		[[nodiscard]] bool is_ignored(const std::filesystem::path& path) const;

		void write_named_file_link(std::wofstream& out, const std::wstring& file_name, const std::wstring& link_name);
	};

	namespace util {

		bool is_valid_path(const std::filesystem::path& path);
		bool is_file(const std::filesystem::path& path);
		bool is_dir(const std::filesystem::path& path);
		bool is_dir_blacklisted(const std::wstring& dir_name, const std::vector<std::wstring>& ignored);

	} // util

} // docs_gen_core

#endif // DOCS_GEN_DIR_H
