#ifndef DOCS_GEN_PARSER_H
#define DOCS_GEN_PARSER_H

#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <unordered_map>

#include "file.hpp"

namespace docs_gen_core {

	class dott_parser {
	public:
		using fields_type = std::unordered_map<std::wstring, std::wstring>;

	private:
		std::filesystem::path root_path_;
		std::shared_ptr<dott_file> file_;

		std::wifstream in_;
		std::wistringstream iss_;
		fields_type fields_;
		std::pair<std::wstring, std::wstring> node_field_;
		std::pair<std::wstring, std::wstring> res_field_;

	public:
		explicit dott_parser(const std::shared_ptr<dott_file>& file);

		[[nodiscard]] const fields_type& get_fields() const { return fields_; }

		bool parse_scene_header();
		bool parse_resource_header();
		bool parse_scene_file_contents(
			const std::unordered_map<std::wstring, std::shared_ptr<scene_file>>& scene_files,
			const std::unordered_map<std::wstring, std::shared_ptr<script_file>>& script_files,
			const std::unordered_map<std::wstring, std::shared_ptr<resource_file>>& resource_files);
		bool parse_resource_file_contents(
			const std::unordered_map<std::wstring, std::shared_ptr<scene_file>>& scene_files,
			const std::unordered_map<std::wstring, std::shared_ptr<script_file>>& script_files,
			const std::unordered_map<std::wstring, std::shared_ptr<resource_file>>& resource_files);

		void set_root_path(const std::filesystem::path& path) { root_path_ = path; }

	private:
		bool next_entry();
		bool next_field();
		bool next_node_field();
		bool next_resource_field();
		
		bool validate_scene_header();
		bool validate_resource_header();
		bool validate_ext_resource_type();
		bool validate_ext_resource_packed_scene();
		bool validate_ext_resource_resource();
		bool validate_ext_resource_script();
		bool validate_ext_resource_other();
		bool validate_sub_resource();
		bool validate_node();
	};

	class script_parser {
		std::shared_ptr<script_file> file_;
		std::wifstream in_;

	public:
		explicit script_parser(const std::shared_ptr<script_file>& file);
		bool parse();

	private:
		std::wstring extract_category_name(const std::wstring& s);
		void extract_and_push_tags(const std::wstring& s, std::vector<std::wstring>& tags);
		script_class::variable extract_variable(const std::wstring& s);
		script_class::function extract_function(const std::wstring& s);
		std::size_t extract_and_push_function_arguments(const std::wstring& s, std::size_t args_start, std::vector<script_class::variable>& vars);
	};

} // docs_gen_core

#endif // DOCS_GEN_PARSER_H
