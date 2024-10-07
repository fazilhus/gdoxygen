#ifndef DOCS_GEN_PARSER_H
#define DOCS_GEN_PARSER_H

#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <unordered_map>

#include "file.hpp"

namespace docs_gen_core {

	class parser {
	public:
		using fields_type = std::unordered_map<std::wstring, std::wstring>;

	private:
		std::filesystem::path root_path_;
		std::shared_ptr<file> file_;

		std::wifstream in_;
		std::wistringstream iss_;
		fields_type fields_;

	public:
		explicit parser(const std::shared_ptr<file>& file);

		[[nodiscard]] const fields_type& get_fields() const { return fields_; }

		bool parse_scene_header();
		bool parse_resource_header();
		bool parse_scene_ext_resources(
			const std::unordered_map<std::wstring, std::shared_ptr<scene_file>>& scene_files,
			const std::unordered_map<std::wstring, std::shared_ptr<script_file>>& script_files,
			const std::unordered_map<std::wstring, std::shared_ptr<resource_file>>& resource_files);
		bool parse_resource_ext_resourcces(
			const std::unordered_map<std::wstring, std::shared_ptr<scene_file>>& scene_files,
			const std::unordered_map<std::wstring, std::shared_ptr<script_file>>& script_files,
			const std::unordered_map<std::wstring, std::shared_ptr<resource_file>>& resource_files);

		void set_root_path(const std::filesystem::path& path) { root_path_ = path; }

	private:
		bool next_entry();
		bool next_field();
		bool validate_scene_header();
		bool validate_resource_header();
		bool validate_ext_resource_type();
		bool validate_ext_resource_packed_scene();
		bool validate_ext_resource_resource();
		bool validate_ext_resource_script();
	};

} // docs_gen_core

#endif // DOCS_GEN_PARSER_H
