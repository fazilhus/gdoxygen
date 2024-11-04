#include "parser.hpp"

#include <iostream>

#include "util/util.hpp"

namespace docs_gen_core {

	dott_parser::dott_parser(const std::shared_ptr<dott_file>& file)
	: file_(file) {
		in_.open(file_->get_path(), std::ios::in | std::ios::binary);
		if (!in_.is_open()) {
			std::cerr << "[ERROR] could not open file: " << file_->get_path() << '\n';
			return;
		}
	}

	bool dott_parser::parse_scene_header() {
		next_entry();
		if (!validate_scene_header()) {
			std::cerr << "[ERROR] corrupted scene file: " << file_->get_path() << '\n';
			return false;
		}

		auto file = dynamic_cast<scene_file*>(file_.get());
		if (!file) {
			std::cerr << "[ERROR] wrong file type " << file_->get_path() << '\n';
			return false;
		}
		
		file->set_uid(fields_[L"uid"]);
		return true;
	}

	bool dott_parser::parse_resource_header() {
		next_entry();
		if (!validate_resource_header()) {
			std::cerr << "[ERROR] corrupted resource file: " << file_->get_path() << '\n';
			return false;
		}

		auto file = dynamic_cast<resource_file*>(file_.get());
		if (!file) {
			std::cerr << "[ERROR] wrong file type " << file_->get_path() << '\n';
			return false;
		}

		file->set_uid(fields_[L"uid"]);
		file->set_script_class(fields_[L"script_class"]);
		return true;
	}

	bool dott_parser::parse_scene_file_contents(
		const std::unordered_map<std::wstring, std::shared_ptr<scene_file>>& scene_files,
		const std::unordered_map<std::wstring, std::shared_ptr<script_file>>& script_files,
		const std::unordered_map<std::wstring, std::shared_ptr<resource_file>>& resource_files) {
		auto file = dynamic_cast<scene_file*>(file_.get());
		if (!file) {
			std::cerr << "[ERROR] wrong file type " << file_->get_path() << '\n';
			return false;
		}
		
		std::cout << "---- Processing scene file " << file->get_path() << " ----\n";
		while (next_entry()) {
			if (fields_.find(L"ext_resource") != fields_.end()) {
				if (!validate_ext_resource_type()) {
					std::cerr << "[ERROR] corrupted scene file (invalid external resource type): " << file->get_path() << '\n';
					return false;
				}

				auto& type = fields_[L"type"];
				if (type == L"PackedScene") {
					if (!validate_ext_resource_packed_scene()) {
						std::cerr << "[WARNING] corrupted scene file (invalid external resource \"PackedScene\"): " << file->get_path() << '\n';
						continue;
					}

					auto& uid = fields_[L"uid"];
					if (scene_files.find(uid) == scene_files.end()) {
						std::cerr << "[WARNING] previously not encountered scene file: ";
						std::wcerr << fields_[L"path"];
						std::cerr << '\n';
						continue;
					}

					file->push_packed_scene(fields_[L"id"], scene_files.at(uid));
				}
				else if (type == L"Script") {
					if (!validate_ext_resource_script()) {
						std::cerr << "[WARNING] corrupted scene file (invalid external resource \"Script\"): " << file->get_path() << '\n';
						continue;
					}

					std::wstring path_str = fields_[L"path"];
					auto rel_root_path = root_path_ / std::filesystem::path{ path_str };
					rel_root_path.make_preferred();
					path_str = rel_root_path.relative_path().wstring();
					if (script_files.find(path_str) == script_files.end()) {
						std::cerr << "[WARNING] previously not encountered script file: ";
						std::wcerr << fields_[L"path"];
						std::cerr << '\n';
						continue;
					}

					file->push_script(fields_[L"id"], script_files.at(path_str));
				}
				else if (type == L"Resource") {
					if (!validate_ext_resource_resource()) {
						std::cerr << "[WARNING] corrupted scene file (invalid external resource \"Resource\"): " << file->get_path() << '\n';
						continue;
					}

					auto& uid = fields_[L"uid"];
					if (resource_files.find(uid) == resource_files.end()) {
						std::cerr << "[WARNING] previously not encountered resource file: ";
						std::wcerr << fields_[L"path"];
						std::cerr << '\n';
						continue;
					}

					file->push_ext_resource(fields_[L"id"], resource_files.at(uid));
				}
				else {
					if (!validate_ext_resource_other()) {
						std::cerr << "[WARNING] corrupted scene file (invalid external resource \"Other\"): " << file->get_path() << '\n';
						continue;
					}

					file->push_ext_resource_other(fields_[L"id"], {fields_[L"type"], fields_[L"path"]});
				}
			}
			else if (fields_.find(L"node") != fields_.end()) {
				auto tn = file->get_node_tree().end();
				
				if (fields_.find(L"type") != fields_.end()) {
					tn = file->get_node_tree().insert(fields_[L"name"], fields_[L"type"], fields_[L"parent"]);
				}
				else if (fields_.find(L"instance") != fields_.end()) {
					auto& instance = fields_[L"instance"];
					if (file->get_packed_scenes().find(instance) != file->get_packed_scenes().end()) {
						tn = file->get_node_tree().insert(fields_[L"name"], L"PackedScene", fields_[L"parent"]);
					}
				}
				else {
					tn = file->get_node_tree().insert(fields_[L"name"], L"Unknown", fields_[L"parent"]);
				}

				if (tn != file->get_node_tree().end()) {
					while (next_node_field()) {
						auto second = node_field_.second;
						if (second.find(L"ExtResource") != std::string::npos) {						
							second = second.substr(13, second.size() - 15);
							const auto& sf =  file->get_packed_scenes().find(second);
							if (sf != file->get_packed_scenes().end()) {
								(*tn)->ext_resource_fields.emplace_back(node_field_.first, sf->second);
								continue;
							}

							const auto& scf = file->get_scripts().find(second);
							if (scf != file->get_scripts().end()) {
								(*tn)->ext_resource_fields.emplace_back(node_field_.first, scf->second);
								continue;
							}

							const auto& rf = file->get_ext_resources().find(second);
							if (rf != file->get_ext_resources().end()) {
								(*tn)->ext_resource_fields.emplace_back(node_field_.first, rf->second);
							}
						}
					}
				}
			}
		}

		return true;
	}

	bool dott_parser::parse_resource_file_contents(
		const std::unordered_map<std::wstring, std::shared_ptr<scene_file>>& scene_files,
		const std::unordered_map<std::wstring, std::shared_ptr<script_file>>& script_files,
		const std::unordered_map<std::wstring, std::shared_ptr<resource_file>>& resource_files) {
		auto file = dynamic_cast<resource_file*>(file_.get());
		if (!file) {
			std::cerr << "[ERROR] wrong file type " << file_->get_path() << '\n';
			return false;
		}

		std::cout << "---- Processing resource file " << file->get_path() << " ----\n";
		while (next_entry()) {
			//if (fields_.find(L"gd_resource") == fields_.end() && fields_.find(L"ext_resource") == fields_.end())
			//	break;
			if (fields_.find(L"ext_resource") != fields_.end()) {
				auto& type = fields_[L"type"];
				if (type == L"Script") {
					if (!validate_ext_resource_script()) {
						std::cerr << "[WARNING] corrupted resource file (invalid external resource \"Script\"): " << file->get_path() << '\n';
						continue;
					}

					std::wstring path_str = fields_[L"path"];
					auto rel_root_path = root_path_ / std::filesystem::path{ path_str };
					rel_root_path.make_preferred();
					path_str = rel_root_path.relative_path().wstring();
					if (script_files.find(path_str) == script_files.end()) {
						std::cerr << "[WARNING] previously not encountered script file: ";
						std::wcerr << fields_[L"path"];
						std::cerr << '\n';
						continue;
					}
				
					file->push_script(fields_[L"id"], script_files.at(path_str));
				}
				else if (type == L"Resource") {
					if (!validate_ext_resource_resource()) {
						std::cerr << "[WARNING] corrupted resource file (invalid external resource \"Resource\"): " << file->get_path() << '\n';
						continue;
					}

					auto& uid = fields_[L"uid"];
					if (resource_files.find(uid) == resource_files.end()) {
						std::cerr << "[WARNING] previously not encountered resource file: ";
						std::wcerr << fields_[L"path"];
						std::cerr << '\n';
						continue;
					}

					file->push_ext_resource(fields_[L"id"], resource_files.at(uid));
				}
				else {
					if (!validate_ext_resource_other()) {
						std::cerr << "[WARNING] corrupted scene file (invalid external resource \"Other\"): " << file->get_path() << '\n';
						continue;
					}

					file->push_ext_resource_other(fields_[L"id"], {fields_[L"type"], fields_[L"path"]});
				}
			}
			else if (fields_.find(L"sub_resource") != fields_.end()) {
				if (!validate_sub_resource()) {
					std::cerr << "[WARNING] corrupted resource file (invalid sub_resource): " << file->get_path() << '\n';
					continue;
				}
				
				const auto& type = fields_[L"type"];
				resource_file::resource r{type, {}, {}, {}, {}};
				while (next_resource_field()) {
					auto& [name, val] = res_field_;
					if (val.find(L"ExtResource") != std::wstring::npos) {
						val = val.substr(13, val.size() - 15);
						const auto& ps = file->get_packed_scenes();
						if (ps.find(val) != ps.end()) {
							r.res_file_fields.push_back({name, ps.at(val)});
							continue;
						}

						const auto& sf = file->get_scripts();
						if (sf.find(val) != sf.end()) {
							r.res_file_fields.push_back({name, sf.at(val)});
							continue;
						}

						const auto& rf = file->get_ext_resources();
						if (rf.find(val) != rf.end()) {
							r.res_file_fields.push_back({name, rf.at(val)});
							continue;
						}
						
						const auto& ero = file->get_ext_resource_other();
						if (ero.find(val) != ero.end()) {
							r.res_other_fields.push_back({name, ero.at(val).name});
						}
					}
					else if (val.find(L"SubResource") != std::wstring::npos) {
						val = val.substr(13, val.size() - 15);
						const auto& srf = file->get_sub_resources();
						if (srf.find(val) != srf.end()) {
							r.sub_res_fields.push_back({name, srf.at(val)});
						}
					}
					else {
						r.fields.push_back({name, val});
					}
				}
				file->push_sub_resource(fields_[L"id"], std::make_shared<resource_file::resource>(r));
			}
			else if (fields_.find(L"resource") != fields_.end()) {
				resource_file::resource r{{},{},{},{}, {}};
				while (next_resource_field()) {
					auto& [name, val] = res_field_;
					if (val.find(L"ExtResource") != std::wstring::npos) {
						val = val.substr(13, val.size() - 15);
						const auto& ps = file->get_packed_scenes();
						if (ps.find(val) != ps.end()) {
							r.res_file_fields.push_back({name, ps.at(val)});
							continue;
						}

						const auto& sf = file->get_scripts();
						if (sf.find(val) != sf.end()) {
							r.res_file_fields.push_back({name, sf.at(val)});
							continue;
						}

						const auto& rf = file->get_ext_resources();
						if (rf.find(val) != rf.end()) {
							r.res_file_fields.push_back({name, rf.at(val)});
							continue;
						}
						
						const auto& ero = file->get_ext_resource_other();
						if (ero.find(val) != ero.end()) {
							r.res_other_fields.push_back({name, ero.at(val).name});
						}
					}
					else if (val.find(L"SubResource") != std::wstring::npos) {
						val = val.substr(13, val.size() - 15);
						const auto& srf = file->get_sub_resources();
						if (srf.find(val) != srf.end()) {
							r.sub_res_fields.push_back({name, srf.at(val)});
						}
					}
					else {
						r.fields.push_back({name, val});
					}
				}
				file->set_resource(r);
			}
		}

		return true;
	}

	bool dott_parser::next_entry() {
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
		in_.seekg(start, std::wifstream::beg);
		std::wstring temp(size, '\0');
		in_.read(temp.data(), size);

		iss_ = std::wistringstream(temp);
		fields_.clear();
		while (next_field()) {
		}

		while (in_.get(c) && c != '\n') {}

		return true;
	}

	bool dott_parser::next_field() {
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
			const auto lhs = temp.substr(0, del);
			auto rhs = temp.substr(del + 1);
			if (lhs == L"uid") {
				rhs = rhs.substr(7, rhs.size() - 8);
			}
			if (lhs == L"path") {
				rhs = rhs.substr(7, rhs.size() - 8);
			}
			if (lhs == L"id" || lhs == L"type" || lhs == L"parent" || lhs == L"name") {
				rhs = rhs.substr(1, rhs.size() - 2);
			}
			if (lhs == L"instance") {
				rhs = rhs.substr(13, rhs.size() - 15);
			}
			fields_[lhs] = rhs;
		}

		return true;
	}

	bool dott_parser::next_node_field() {
		if (in_.eof() || in_.bad())
			return false;
		
		std::wstring temp;
		std::getline(in_, temp);

		if (temp.empty())
			return false;

		auto del = temp.find_first_of('=');
		if (del == std::string::npos)
			return false;

		node_field_ = {temp.substr(0, del - 1), temp.substr(del + 2)};

		return true;
	}

	bool dott_parser::next_resource_field() {
		if (in_.eof() || in_.bad())
			return false;
		
		std::wstring temp;
		std::getline(in_, temp);

		if (temp.empty())
			return false;

		auto del = temp.find_first_of('=');
		if (del == std::string::npos)
			return false;

		res_field_ = {temp.substr(0, del - 1), temp.substr(del + 2)};

		return true;
	}

	// TODO think of a more sophisticated validation lul
	bool dott_parser::validate_scene_header() {
		return fields_.find(L"gd_scene") != fields_.end()
			&& fields_.find(L"uid") != fields_.end() && !fields_[L"uid"].empty();
	}

	bool dott_parser::validate_resource_header() {
		return fields_.find(L"gd_resource") != fields_.end()
			&& fields_.find(L"uid") != fields_.end() && !fields_[L"uid"].empty();
	}

	bool dott_parser::validate_ext_resource_type() {
		return fields_.find(L"type") != fields_.end() && !fields_[L"type"].empty();
	}

	bool dott_parser::validate_ext_resource_packed_scene() {
		return fields_.find(L"uid") != fields_.end() && !fields_[L"uid"].empty()
			&& fields_.find(L"path") != fields_.end() && !fields_[L"path"].empty() 
			&& fields_.find(L"id") != fields_.end() && !fields_[L"id"].empty();
	}

	bool dott_parser::validate_ext_resource_resource() {
		return fields_.find(L"uid") != fields_.end() && !fields_[L"uid"].empty()
			&& fields_.find(L"path") != fields_.end() && !fields_[L"path"].empty() 
			&& fields_.find(L"id") != fields_.end() && !fields_[L"id"].empty();
	}

	bool dott_parser::validate_ext_resource_script() {
		return fields_.find(L"path") != fields_.end() && !fields_[L"path"].empty()
			&& fields_.find(L"id") != fields_.end() && !fields_[L"id"].empty();
	}

	bool dott_parser::validate_ext_resource_other() {
		return fields_.find(L"path") != fields_.end() && !fields_[L"path"].empty()
			&& fields_.find(L"id") != fields_.end() && !fields_[L"id"].empty();
	}

	bool dott_parser::validate_sub_resource() {
		return fields_.find(L"type") != fields_.end() && !fields_[L"type"].empty()
			&& fields_.find(L"id") != fields_.end() && !fields_[L"id"].empty();
	}

	bool dott_parser::validate_node() {
		return fields_.find(L"name") != fields_.end() && !fields_[L"name"].empty()
			&& (fields_.find(L"type") != fields_.end() && !fields_[L"type"].empty()
			|| fields_.find(L"instance") != fields_.end() && !fields_[L"instance"].empty());
	}

	script_parser::script_parser(const std::shared_ptr<script_file>& file)
		: file_(file) {
		in_.open(file_->get_path(), std::ios::in | std::ios::binary);
		if (!in_.is_open()) {
			std::cerr << "[ERROR] could not open file: " << file_->get_path() << '\n';
			return;
		}
	}

	bool script_parser::parse() {
		std::wstring line;
		script_class sc{};
		while (std::getline(in_, line)) {
			if (line.empty())
				continue;

			if (line.find(L"extends") != std::string::npos) {
				sc.parent = line.substr(8);
				continue;
			}

			if (line.find(L"#CLASS") != std::string::npos) {
				sc.short_desc = line.substr(7);
				continue;
			}
			
			if (line.find(L"class_name") != std::string::npos) {
				sc.name = line.substr(11);
				continue;
			}

			if (line.find(L"#TAGS") != std::string::npos) {
				extract_and_push_tags(line, sc.tags);
				continue;
			}

			if (line.find(L"@export_category") != std::string::npos) {
				sc.categories.emplace_back(script_class::export_category{ extract_category_name(line), {} });
				continue;
			}

			if (line.find(L"#VAR") != std::string::npos) {
				if (sc.categories.empty()) {
					sc.categories.emplace_back(script_class::export_category{ {}, {} });
				}
				auto& cat = sc.categories.back();
				auto var_desc = line.substr(5);

				std::getline(in_, line);
				if (line.find(L"@export var") == std::string::npos) {
					return false;
				}

				auto v = extract_variable(line);
				v.short_desc = var_desc;
				cat.variables.emplace_back(v);
				
				continue;
			}
			
			if (line.find(L"@export var") != std::string::npos) {
				if (sc.categories.empty()) {
					sc.categories.emplace_back(script_class::export_category{ {}, {} });
				}
				auto& cat = sc.categories.back();
				cat.variables.emplace_back(extract_variable(line));
				continue;
			}

			if (line.find(L"#FUNC") != std::string::npos) {
				auto func_desc = line.substr(6);

				std::getline(in_, line);
				if (line.find(L"func") == std::string::npos) {
					return false;
				}
				auto f = extract_function(line);
				f.short_desc = func_desc;
				sc.functions.emplace_back(f);
				continue;
			}

			if (line[0] == 'f' && line[1] == 'u' && line[2] == 'n' && line[3] == 'c') {
				sc.functions.emplace_back(extract_function(line));
			}
		}
		file_->set_script_class(std::move(sc));
		return true;
	}

	std::wstring script_parser::extract_category_name(const std::wstring& s) {
		std::wstring res;
		std::size_t start = s.find_first_of('"');
		std::size_t stop = s.find_last_of('"');
		return s.substr(start + 1, stop - start - 1);
	}

	void script_parser::extract_and_push_tags(const std::wstring& s, std::vector<std::wstring>& tags) {
		std::wstring token;
		for (std::size_t i = 6; i < s.size(); ++i) {
			if (s[i] == ',') {
				tags.emplace_back(token);
				token.clear();
				i += 1;
				continue;
			}

			token += s[i];
		}

		tags.emplace_back(token);
	}

	script_class::variable script_parser::extract_variable(const std::wstring& s) {
		std::wstring name, type;
		std::size_t i;
		for (i = 12; i < s.size() && s[i] != ':'; ++i) {
			if (std::isblank(s[i]))
				continue;
			name += s[i];
		}

		for (i += 1; i < s.size() && s[i] != '='; ++i) {
			if (std::isblank(s[i]))
				continue;
			type += s[i];
		}

		return {name, type, {}};
	}

	script_class::function script_parser::extract_function(const std::wstring& s) {
		script_class::function res;

		std::size_t i;
		for (i = 5; i < s.size() && s[i] != '('; ++i) {
			if (std::isblank(s[i]))
				continue;

			res.name += s[i];
		}
		
		i = extract_and_push_function_arguments(s, i + 1, res.arguments);

		for (; i < s.size() - 1; ++i) {
			if (s[i] == '-' && s[i + 1] == '>') {
				i += 2;
				break;
			}
		}

		for (; i < s.size() && s[i] != ':'; ++i) {
			if (std::isspace(s[i]))
				continue;

			res.return_type += s[i];
		}
		if (res.return_type.empty())
			res.return_type = L"void";
		
		return res;
	}

	std::size_t script_parser::extract_and_push_function_arguments(const std::wstring& s, std::size_t args_start,
		std::vector<script_class::variable>& vars) {
		std::size_t args_end = s.find(')', args_start);

		std::vector<std::wstring> args;
		util::split_by(s.substr(args_start, args_end - args_start), ',', args);
		for (const auto& arg : args) {
			std::size_t delim = arg.find(':');
			if (delim == std::string::npos) {
				vars.emplace_back(script_class::variable{arg, {}, {}});
			}
			else {
				vars.emplace_back(script_class::variable{
					arg.substr(0, delim),
					arg.substr(delim + 1),
					{}});
			}
		}
		
		return args_end + 1;
	}
} // docs_gen_core
