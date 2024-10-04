#include "scene_parser.hpp"

#include <iostream>

namespace core {

	scene_parser::scene_parser(const std::shared_ptr<scene_file>& file)
	: file_(file) {
		in_.open(file_->get_path(), std::ios::in | std::ios::binary);
		if (!in_.is_open()) {
			std::cerr << "[ERROR] could not open file: " << file_->get_path() << '\n';
			return;
		}
	}

	bool scene_parser::parse_scene_header() {
		next_entry();
		if (!fields_.contains(L"gd_scene")) {
			std::cerr << "[ERROR] corrupted scene file: " << file_->get_path() << '\n';
			return false;
		}

		if (!fields_.contains(L"uid")) {
			std::cerr << "[ERROR] corrupted scene file: " << file_->get_path() << '\n';
			return false;
		}

		auto& uid = fields_[L"uid"];
		if (uid.empty()) {
			std::cerr << "[ERROR] corrupted scene file: " << file_->get_path() << '\n';
			return false;
		}

		file_->set_uid(uid);
		return true;
	}

	bool scene_parser::parse_scene_ext_resources(
		const std::unordered_map<std::wstring, std::shared_ptr<scene_file>>& scene_files,
		const std::unordered_map<std::wstring, std::shared_ptr<script_file>>& script_files) {
		std::cout << "---- Processing file " << file_->get_path() << " ----\n";
		while (next_entry()) {
			if (!fields_.contains(L"gd_scene") && !fields_.contains(L"ext_resource"))
				break;

			if (fields_.contains(L"ext_resource")) {
				if (!validate_resource_type()) {
					std::cerr << "[ERROR] corrupted scene file (invalid external resource type): " << file_->get_path() << '\n';
					return false;
				}

				auto& type = fields_[L"type"];
				if (type == L"PackedScene") {
					if (!validate_resource_packed_scene()) {
						std::cerr << "[WARNING] skipping corrupted scene file (invalid external resource \"PackedScene\"): " << file_->get_path() << '\n';
						continue;
					}

					auto& uid = fields_[L"uid"];
					if (!scene_files.contains(uid)) {
						std::cerr << "[WARNING] previously not encountered scene file: ";
						std::wcerr << fields_[L"path"];
						std::cerr << '\n';
						continue;
					}

					file_->push_child(scene_files.at(uid));
				}
				else if (type == L"Script") {
					if (!validate_resource_script()) {
						std::cerr << "[WARNING] skipping corrupted scene file (invalid external resource \"Script\"): " << file_->get_path() << '\n';
						continue;
					}

					std::wstring path_str = fields_[L"path"];
					auto rel_root_path = root_path_ / std::filesystem::path{ path_str };
					rel_root_path.make_preferred();
					path_str = rel_root_path.relative_path().wstring();
					if (!script_files.contains(path_str)) {
						std::cerr << "[WARNING] previously not encountered script file: ";
						std::wcerr << fields_[L"path"];
						std::cerr << '\n';
						continue;
					}

					file_->push_script(script_files.at(path_str));
				}
				else {
					std::cerr << "[WARNING] external resource of type ";
					std::wcerr << fields_[L"path"];
					std::cerr << " not supported\n";
				}
			}
		}

		return true;
	}

	bool scene_parser::next_entry() {
		if (in_.eof() || in_.bad())
			return false;

		wchar_t c;
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
		std::wstring temp(size, '\0');
		in_.read(temp.data(), size);

		iss_ = std::wistringstream(temp);
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

	bool scene_parser::next_field() {
		if (iss_.eof())
			return false;

		std::wstring temp;
		wchar_t c;
		bool is_quote_opened = false;
		while (iss_.get(c)) {
			if (c == '"') {
				is_quote_opened = !is_quote_opened;
			}
			if (std::isspace(c) && !is_quote_opened) {
				break;
			}
			temp.push_back(c);
		}

		auto del = temp.find_first_of('=');
		if (del == std::string::npos) {
			fields_[temp] = {};
		} else {
			auto lhs = temp.substr(0, del);
			auto rhs = temp.substr(del + 1);
			if (lhs == L"uid") {
				rhs = rhs.substr(7, rhs.size() - 8);
			}
			if (lhs == L"path") {
				rhs = rhs.substr(7, rhs.size() - 8);
			}
			if (lhs == L"id" || lhs == L"type") {
				rhs = rhs.substr(1, rhs.size() - 2);
			}
			fields_[lhs] = rhs;
		}

		return true;
	}

	// TODO think of a more sophisticated validation lul
	bool scene_parser::validate_resource_type() {
		return fields_.contains(L"type") && !fields_[L"type"].empty();
	}

	bool scene_parser::validate_resource_packed_scene() {
		return fields_.contains(L"uid") && !fields_[L"uid"].empty()
			&& fields_.contains(L"path") && !fields_[L"path"].empty() 
			&& fields_.contains(L"id") && !fields_[L"id"].empty();
	}

	bool scene_parser::validate_resource_script() {
		return fields_.contains(L"path") && !fields_[L"path"].empty()
			&& fields_.contains(L"id") && !fields_[L"id"].empty();
	}
} // core
