#include "dir.hpp"

#include <iostream>
#include <ranges>

#include "scene_file_parser.hpp"

namespace core {
	bool dir::set_path(const std::string& path) {
		const auto temp = std::filesystem::path(path);

		if (!exists(temp)) {
			return false;
		}

		path_ = temp;
		std::vector<std::shared_ptr<scene_file>> scene_files;
		//std::vector<std::shared_ptr<script_file>> script_files;
		std::vector<std::shared_ptr<resource_file>> resource_files;
		for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ path_ }) {
			if (dir_entry.is_regular_file()) {
				if (dir_entry.path().extension() == ".gd" || dir_entry.path().extension() == ".cs") {
					auto f = script_file(dir_entry.path());
					script_files_[dir_entry.path().relative_path().string()] = std::make_shared<script_file>(f);
				}
				
				if (dir_entry.path().extension() == ".tscn") {
					auto f = scene_file(dir_entry.path());
					scene_files.push_back(std::make_shared<scene_file>(f));
				}

				if (dir_entry.path().extension() == ".tres") {
					auto f = resource_file(dir_entry.path());
					resource_files.push_back(std::make_shared<resource_file>(f));
				}
			}
		}

		for (auto& val : scene_files) {
			scene_file_parser p{ val };
			p.parse_scene_header();
			scene_files_[val->get_uid()] = val;
		}

		for (auto& val : scene_files) {
			scene_file_parser p{ val };
			p.set_root_path(path_);
			p.parse_scene_ext_resources(scene_files_, script_files_);
		}
		
		for (const auto& val : scene_files_ | std::views::values) {
			std::cout << "scene " << val->get_title() << " uses:\n";
			for (const auto& file : val->get_children()) {
				std::cout << "\tscene " << file->get_title() << '\n';
			}
			for (const auto& file : val->get_scripts()) {
				std::cout << "\tscript " << file->get_title() << '\n';
			}
		}

		return true;
	}

	bool dir::is_file() const {
		return is_regular_file(path_);
	}

	bool dir::is_dir() const {
		return is_directory(path_);
	}
} // core
