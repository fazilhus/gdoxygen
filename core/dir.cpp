#include "dir.hpp"

#include <iostream>
#include <ranges>

#include "scene_file_parser.hpp"

namespace core {
	bool dir::set_path(const std::string& path) {
		const auto temp = std::filesystem::path(path);

		if (!util::is_valid_path(temp)) {
			return false;
		}

		path_ = temp;
		return true;
	}

	void dir::construct_file_tree() {
		std::vector<std::shared_ptr<scene_file>> scene_files;
		std::vector<std::shared_ptr<resource_file>> resource_files;
		for (auto dir_entry = std::filesystem::recursive_directory_iterator(path_); dir_entry != std::filesystem::recursive_directory_iterator(); ++dir_entry) {
			//std::cout << *dir_entry << '\n';
			if (util::is_dir(*dir_entry) && std::ranges::find(ignored_folders_, dir_entry->path().filename()) != ignored_folders_.end()) {
				//std::cout << "\tignored\n";
				dir_entry.disable_recursion_pending();
				continue;
			}

			if (util::is_file(*dir_entry)) {
				if (dir_entry->path().extension() == ".gd" || dir_entry->path().extension() == ".cs") {
					auto f = script_file(dir_entry->path());
					script_files_[dir_entry->path().relative_path().string()] = std::make_shared<script_file>(f);
				}

				if (dir_entry->path().extension() == ".tscn") {
					auto f = scene_file(dir_entry->path());
					scene_files.push_back(std::make_shared<scene_file>(f));
				}

				if (dir_entry->path().extension() == ".tres") {
					auto f = resource_file(dir_entry->path());
					resource_files.push_back(std::make_shared<resource_file>(f));
				}
			}
		}

		for (auto& val : scene_files) {
			scene_file_parser p{ val };
			if (!p.parse_scene_header())
				return;
			file_tree_[val->get_uid()] = val;
		}

		for (auto& val : scene_files) {
			scene_file_parser p{ val };
			p.set_root_path(path_);
			if (!p.parse_scene_ext_resources(file_tree_, script_files_))
				return;
		}

		//for (const auto& val : file_tree_ | std::views::values) {
		//	std::cout << "scene " << val->get_title() << " uses:\n";
		//	for (const auto& file : val->get_children()) {
		//		std::cout << "\tscene " << file->get_title() << '\n';
		//	}
		//	for (const auto& file : val->get_scripts()) {
		//		std::cout << "\tscript " << file->get_title() << '\n';
		//	}
		//}
	}

	void dir::gen_docs() {
		auto docs_dir = path_ / "docs";
		if (std::filesystem::exists(docs_dir)) {
			std::filesystem::remove_all(docs_dir);
		}

		std::filesystem::create_directory(docs_dir);
		
		for (const auto& file : file_tree_ | std::views::values) {
			auto doc_path = file->get_path();
			doc_path = std::filesystem::relative(doc_path, path_);
			doc_path = docs_dir / doc_path;
			std::filesystem::create_directory(doc_path.parent_path());
			doc_path.replace_extension(".md");
			std::ofstream out{ doc_path, std::ios::out | std::ios::binary };

			for (const auto& child : file->get_children()) {
				auto child_doc_path = std::filesystem::relative(child->get_path(), path_);
				child_doc_path = docs_dir / child_doc_path;
				child_doc_path.replace_extension(".md");
				out.write("[[", 2);
				auto child_filename = child_doc_path.filename().string();
				out.write(child_filename.c_str(), child_filename.size());
				out.write("]]\n", 3);
			}

			for (const auto& child : file->get_scripts()) {
				auto script_doc_path = std::filesystem::relative(child->get_path(), path_);
				script_doc_path = docs_dir / script_doc_path;
				script_doc_path.replace_filename(script_doc_path.filename().string() + ".md");
				out.write("[[", 2);
				auto script_filename = script_doc_path.filename().string();
				out.write(script_filename.c_str(), script_filename.size());
				out.write("]]\n", 3);
			}

			out.close();
		}

		for (const auto& file : script_files_ | std::views::values) {
			auto doc_path = file->get_path();
			doc_path = std::filesystem::relative(doc_path, path_);
			doc_path = docs_dir / doc_path;
			std::filesystem::create_directory(doc_path.parent_path());
			doc_path.replace_filename(doc_path.filename().string() + ".md");
			std::ofstream out{ doc_path, std::ios::out | std::ios::binary };
			out.close();
		}

	}

	bool dir::is_ignored(const std::filesystem::path& path) const {
		for (const auto& ignored : ignored_folders_) {
			if (path.compare(ignored) == 0) {
				std::cout << "\tshould be ignored\n";
				return true;
			}

			auto rel = std::filesystem::relative(path, ignored);
			std::cout << path << '\n';
			if (!rel.empty() && rel.native()[0] != '.') {
				std::cout << "\tshould be ignored\n";
				return true;
			}
		}

		return false;
	}

	namespace util {

		bool is_valid_path(const std::filesystem::path& path) {
			return std::filesystem::exists(path);
		}

		bool is_file(const std::filesystem::path& path) {
			return std::filesystem::is_regular_file(path);
		}

		bool is_dir(const std::filesystem::path& path) {
			return std::filesystem::is_directory(path);
		}

	} // util

} // core
