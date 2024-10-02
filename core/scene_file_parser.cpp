#include "scene_file_parser.hpp"

#include <iostream>

namespace core {

	scene_file_parser::scene_file_parser(const std::shared_ptr<scene_file>& file)
	: file_(file) {
		in_.open(file_->get_path(), std::ios::in | std::ios::binary);
		if (!in_.is_open()) {
			std::cerr << "[ERROR] could not open file: " << file_->get_path() << '\n';
			return;
		}
	}

	bool scene_file_parser::parse_scene_header() {
		next_entry();
		if (!fields_.contains("gd_scene")) {
			std::cerr << "[ERROR] corrupted scene file: " << file_->get_path() << '\n';
			return false;
		}

		if (!fields_.contains("uid")) {
			std::cerr << "[ERROR] corrupted scene file: " << file_->get_path() << '\n';
			return false;
		}

		auto& uid = fields_["uid"];
		if (uid.empty()) {
			std::cerr << "[ERROR] corrupted scene file: " << file_->get_path() << '\n';
			return false;
		}

		file_->set_uid(uid);
		return true;
	}

	bool scene_file_parser::parse_scene_ext_resources(
		const std::unordered_map<std::string, std::shared_ptr<scene_file>>& scene_files,
		const std::unordered_map<std::string, std::shared_ptr<script_file>>& script_files) {
		//std::cout << "---- Processing file " << file_->get_path() << '\n';
		while (next_entry()) {
			if (!fields_.contains("gd_scene") && !fields_.contains("ext_resource"))
				break;

			if (fields_.contains("ext_resource")) {
				if (!validate_resource_type()) {
					std::cerr << "[ERROR] corrupted scene file (invalid external resource type): " << file_->get_path() << '\n';
					return false;
				}

				auto& type = fields_["type"];
				if (type == "PackedScene") {
					if (!validate_resource_packed_scene()) {
						std::cerr << "[ERROR] corrupted scene file (invalid external resource \"PackedScene\"): " << file_->get_path() << '\n';
						return false;
					}

					auto& uid = fields_["uid"];
					if (!scene_files.contains(uid)) {
						std::cerr << "[WARNING] previously not encountered scene file: " << fields_["path"] << '\n';
						continue;
					}

					file_->push_child(scene_files.at(uid));
				}
				else if (type == "Script") {
					if (!validate_resource_script()) {
						std::cerr << "[ERROR] corrupted scene file (invalid external resource \"Script\"): " << file_->get_path() << '\n';
						return false;
					}

					std::string path_str = fields_["path"];
					auto rel_root_path = root_path_ / std::filesystem::path{ path_str };
					rel_root_path.make_preferred();
					path_str = rel_root_path.relative_path().string();
					if (!script_files.contains(path_str)) {
						std::cerr << "[WARNING] previously not encountered script file: " << fields_["path"] << '\n';
						continue;
					}

					file_->push_script(script_files.at(path_str));
				}
				else {
					std::cout << "[WARNING] external resource of type " << std::quoted(type) << " not supported\n";
				}
			}
		}

		return true;
	}

	bool scene_file_parser::next_entry() {
		if (in_.eof() || in_.bad())
			return false;

		char c;
		while (in_.get(c) && c != '[') {
			if (c == '{') {
				while (in_.get(c) && c != '}') {}
			}
		}
		if (in_.eof() || in_.bad())
			return false;

		const auto start = in_.tellg();
		while (in_.get(c) && c != ']') {}
		if (in_.eof() || in_.bad())
			return false;

		const auto size = in_.tellg() - start - 1;
		in_.seekg(start, in_.beg);
		std::string temp(size, '\0');
		in_.read(temp.data(), size);

		iss_ = std::istringstream(temp);
		fields_.clear();
		while (next_field()) {
			//field_type field = fields_.back();
			//if (field.first == "uid") {
			//	file_->set_uid(field.second);
			//}
			//std::cout << "Key " << fields_.back().first << " Val " << fields_.back().second << '\n';
		}

		return true;
	}

	bool scene_file_parser::next_field() {
		if (iss_.eof())
			return false;

		std::string temp;
		char c;
		while (iss_.get(c) && !std::isspace(c)) {
			temp.push_back(c);
		}

		auto del = temp.find_first_of('=');
		if (del == std::string::npos) {
			fields_[temp] = "";
		} else {
			auto lhs = temp.substr(0, del);
			auto rhs = temp.substr(del + 1);
			if (lhs == "uid") {
				rhs = rhs.substr(7, rhs.size() - 8);
			}
			if (lhs == "path") {
				rhs = rhs.substr(7, rhs.size() - 8);
			}
			if (lhs == "id" || lhs == "type") {
				rhs = rhs.substr(1, rhs.size() - 2);
			}
			fields_[lhs] = rhs;
		}

		return true;
	}

	// TODO think of a more sophisticated validation lul
	bool scene_file_parser::validate_resource_type() {
		return fields_.contains("type") && !fields_["type"].empty();
	}

	bool scene_file_parser::validate_resource_packed_scene() {
		return fields_.contains("uid") && !fields_["uid"].empty()
			&& fields_.contains("path") && !fields_["path"].empty() 
			&& fields_.contains("id") && !fields_["id"].empty();
	}

	bool scene_file_parser::validate_resource_script() {
		return fields_.contains("path") && !fields_["path"].empty()
			&& fields_.contains("id") && !fields_["id"].empty();
	}
} // core
