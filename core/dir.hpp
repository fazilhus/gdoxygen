#pragma once

#include <filesystem>
#include <vector>

#include <memory>
#include <unordered_map>

#include "file.hpp"

namespace core {

	class dir {
		std::filesystem::path path_;
		std::vector<std::string> ignored_folders_;

		std::unordered_map<std::string, std::shared_ptr<scene_file>> file_tree_;
		std::unordered_map<std::string, std::shared_ptr<script_file>> script_files_;

	public:
		dir() = default;
		dir(const dir& other) = delete;
		dir(dir&& other) = delete;
		~dir() = default;

		dir& operator=(const dir& other) = delete;
		dir& operator=(dir&& other) = delete;

		[[nodiscard]] bool set_path(const std::string& path);
		void set_ignored_folders(const std::vector<std::string>& folders) { ignored_folders_ = folders; }
		void push_ignored_folder(const std::string& folder) { ignored_folders_.push_back(folder); }

		void construct_file_tree();
		void gen_docs();

	private:
		[[nodiscard]] bool is_ignored(const std::filesystem::path& path) const;
	};

	namespace util {

		[[nodiscard]] bool is_valid_path(const std::filesystem::path& path);
		[[nodiscard]] bool is_file(const std::filesystem::path& path);
		[[nodiscard]] bool is_dir(const std::filesystem::path& path);

	} // util

} // core
