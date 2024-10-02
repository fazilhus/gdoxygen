#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "file.hpp"

namespace core {

	class scene_file_parser {
	public:
		using fields_type = std::unordered_map<std::string, std::string>;

	private:
		std::filesystem::path root_path_;
		std::shared_ptr<scene_file> file_;

		std::ifstream in_;
		std::istringstream iss_;
		fields_type fields_;

	public:
		explicit scene_file_parser(const std::shared_ptr<scene_file>& file);

		[[nodiscard]] const fields_type& get_fields() const { return fields_; }

		bool parse_scene_header();
		bool parse_scene_ext_resources(
			const std::unordered_map<std::string, std::shared_ptr<scene_file>>& scene_files,
			const std::unordered_map<std::string, std::shared_ptr<script_file>>& script_files);

		void set_root_path(const std::filesystem::path& path) { root_path_ = path; }

	private:
		bool next_entry();
		bool next_field();
		bool validate_resource_type();
		bool validate_resource_packed_scene();
		bool validate_resource_script();
	};

} // core
