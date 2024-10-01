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

	void scene_file_parser::parse_scene_header() {
		next_entry();
		if (fields_.front().first == "gd_scene") {
			for (const auto& [key, val] : fields_) {
				if (key == "uid") {
					file_->set_uid(val);
				}
			}
		} else {
			std::cerr << "[ERROR] corrupted scene file: " << file_->get_path() << '\n';
		}
	}

	void scene_file_parser::parse_scene_ext_resources(
		const std::unordered_map<std::string, std::shared_ptr<scene_file>>& scene_files,
		const std::unordered_map<std::string, std::shared_ptr<script_file>>& script_files) {
		while (next_entry()) {
			if (fields_[0].first != "gd_scene" && fields_[0].first != "ext_resource") break;

			if (fields_[0].first == "ext_resource") {
				if (fields_[1].first == "type") {
					if (fields_[1].second == "PackedScene") {
						// ext_resource type=PackedScene uid path id
						if (scene_files.contains(fields_[2].second)) {
							file_->push_child(scene_files.at(fields_[2].second));
						}
					}
					if (fields_[1].second == "Script") {
						// ext_resource type=Script path id
						auto path_str = fields_[2].second;
						auto rel_root_path = root_path_ / std::filesystem::path{ path_str };
						rel_root_path.make_preferred();
						path_str = rel_root_path.relative_path().string();
						if (script_files.contains(path_str)) {
							file_->push_script(script_files.at(path_str));
						}
					}
				}
			}

		}
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
			field_type field = fields_.back();
			if (field.first == "uid") {
				file_->set_uid(field.second);
			}
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
			fields_.emplace_back(temp, "");
		} else {
			auto lhs = temp.substr(0, del);
			auto rhs = temp.substr(del + 1);
			if (lhs == "uid") {
				rhs = rhs.substr(7, rhs.size() - 8);
			}
			if (lhs == "path") {
				rhs = rhs.substr(7, rhs.size() - 8);
				//rhs = root_path_.append()
			}
			if (lhs == "id" || lhs == "type") {
				rhs = rhs.substr(1, rhs.size() - 2);
			}
			fields_.emplace_back(lhs, rhs);
		}

		return true;
	}

} // core
