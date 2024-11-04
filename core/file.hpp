#ifndef DOCS_GEN_FILE_H
#define DOCS_GEN_FILE_H

#include <filesystem>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "node.hpp"

namespace docs_gen_core {

	class file {
	protected:
		std::filesystem::path path_;
		std::wstring title_;

		file() = default;
		explicit file(const std::filesystem::path& path);
		file(const file& other);
		file(file&& other) noexcept;
		virtual ~file() = default;
	
	public:
		file& operator=(const file&) = delete;
		file& operator=(file&&) = delete;
		
		[[nodiscard]] const std::filesystem::path& get_path() const { return path_; }
		[[nodiscard]] const std::wstring& get_title() const { return title_; }
	};

	struct script_class {
		struct variable {
			std::wstring name;
			std::wstring type;
			std::wstring short_desc;
		};

		struct export_category {
			std::wstring name;
			std::vector<variable> variables;
		};
		
		struct function {
			std::wstring name;
			std::wstring short_desc;
			std::vector<variable> arguments;
			std::wstring return_type;
		};
		
		bool is_public;
		std::wstring name;
		std::wstring parent;
		std::vector<std::wstring> tags;
		std::wstring short_desc;
		std::vector<export_category> categories;
		std::vector<function> functions;
	};

	class script_file final : public file {
		script_class class_;
		
	public:
		script_file() = default;
		explicit script_file(const std::filesystem::path& path);
		script_file(const script_file& other);
		script_file(script_file&& other) noexcept;
		~script_file() override = default;

		script_file& operator=(const script_file& other);
		script_file& operator=(script_file&& other) noexcept;

		void set_script_class(const script_class& c) { class_ = c; }
		void set_script_class(script_class&& c) { class_ = std::move(c); }
		[[nodiscard]] const script_class& get_script_class() const { return class_; }
	};

	class scene_file;
	class resource_file;

	struct ext_resource_other {
		std::wstring type;
		std::filesystem::path path;
		std::wstring name;

		ext_resource_other() = default;
		ext_resource_other(const std::wstring& type, const std::filesystem::path& path);
		ext_resource_other(const ext_resource_other&) = default;
		ext_resource_other(ext_resource_other&&) noexcept = default;
		~ext_resource_other() = default;

		ext_resource_other& operator=(const ext_resource_other&) = default;
		ext_resource_other& operator=(ext_resource_other&&) noexcept = default;
	};

	class dott_file : public file {
	protected:
		std::unordered_map<std::wstring, std::shared_ptr<scene_file>> packed_scenes_;
		std::unordered_map<std::wstring, std::shared_ptr<resource_file>> ext_resources_;
		std::unordered_map<std::wstring, ext_resource_other> ext_resources_other_;

		dott_file() = default;
		explicit dott_file(const std::filesystem::path& path);
		dott_file(const dott_file& other);
		dott_file(dott_file&& other) noexcept;
		~dott_file() override = default;

		dott_file& operator=(const dott_file& other);
		dott_file& operator=(dott_file&& other) noexcept;

	public:
		void push_packed_scene(const std::wstring& key, const std::shared_ptr<scene_file>& child);
		void push_ext_resource(const std::wstring& key, const std::shared_ptr<resource_file>& resource);
		void push_ext_resource_other(const std::wstring& key, const ext_resource_other& resource);

		[[nodiscard]] const std::unordered_map<std::wstring, std::shared_ptr<scene_file>>& get_packed_scenes() const { return packed_scenes_; }
		[[nodiscard]] std::unordered_map<std::wstring, std::shared_ptr<scene_file>>& get_packed_scenes() { return packed_scenes_; }
		[[nodiscard]] const std::unordered_map<std::wstring, std::shared_ptr<resource_file>>& get_ext_resources() const { return ext_resources_; }
		[[nodiscard]] std::unordered_map<std::wstring, std::shared_ptr<resource_file>>& get_ext_resources() { return ext_resources_; }
		[[nodiscard]] const std::unordered_map<std::wstring, ext_resource_other>& get_ext_resource_other() const { return ext_resources_other_; }
		[[nodiscard]] std::unordered_map<std::wstring, ext_resource_other>& get_ext_resource_other() { return ext_resources_other_; }
	};

	class resource_file final : public dott_file {
	public:
		struct resource {
			struct field {
				std::wstring name;
				std::wstring value;
			};
			struct sub_res_field {
				std::wstring name;
				std::weak_ptr<resource> field;
			};
			struct ext_res_field {
				std::wstring name;
				std::weak_ptr<file> file;
			};

			std::wstring type;
			std::vector<ext_res_field> res_file_fields;
			std::vector<field> res_other_fields;
			std::vector<sub_res_field> sub_res_fields;
			std::vector<field> fields;
		};

	private:
		std::wstring uid_;
		std::wstring script_class_;
		std::unordered_map<std::wstring, std::shared_ptr<script_file>> scripts_;

		std::unordered_map<std::wstring, std::shared_ptr<resource>> sub_resources_;
		resource resource_;

	public:
		resource_file() = default;
		explicit resource_file(const std::filesystem::path& path);
		resource_file(const resource_file& other);
		resource_file(resource_file&& other) noexcept;
		~resource_file() override = default;

		resource_file& operator=(const resource_file& other);
		resource_file& operator=(resource_file&& other) noexcept;

		void set_uid(const std::wstring& s) { uid_ = s; }
		void set_script_class(const std::wstring& s) { script_class_ = s; }
		void push_script(const std::wstring& key, const std::shared_ptr<script_file>& s);
		void push_sub_resource(const std::wstring& key, const std::shared_ptr<resource>& resource);
		void set_resource(const resource& resource) { resource_ = resource; }

		[[nodiscard]] const std::wstring& get_uid() const { return uid_; }
		[[nodiscard]] const std::wstring& get_script_class() const { return script_class_; }
		[[nodiscard]] const std::unordered_map<std::wstring, std::shared_ptr<script_file>>& get_scripts() const { return scripts_; }
		[[nodiscard]] const std::unordered_map<std::wstring, std::shared_ptr<resource>>& get_sub_resources() const { return sub_resources_; }
		[[nodiscard]] const resource& get_resource() const { return resource_; }
	};

	class scene_file final : public dott_file {
		std::wstring uid_;
		std::unordered_map<std::wstring, std::shared_ptr<script_file>> scripts_;
		node_tree node_tree_;

	public:
		scene_file() = default;
		explicit scene_file(const std::filesystem::path& path);
		scene_file(const scene_file& other);
		scene_file(scene_file&& other) noexcept;
		~scene_file() override = default;

		scene_file& operator=(const scene_file& other);
		scene_file& operator=(scene_file&& other) noexcept;

		void set_uid(const std::wstring& s) { uid_ = s; }
		void push_script(const std::wstring& key, const std::shared_ptr<script_file>& script);

		[[nodiscard]] const std::wstring& get_uid() const { return uid_; }
		[[nodiscard]] const std::unordered_map<std::wstring, std::shared_ptr<script_file>>& get_scripts() const { return scripts_; }
		[[nodiscard]] std::unordered_map<std::wstring, std::shared_ptr<script_file>>& get_scripts() { return scripts_; }
		[[nodiscard]] const node_tree& get_node_tree() const { return node_tree_; }
		[[nodiscard]] node_tree& get_node_tree() { return node_tree_; }
	};

	struct scene_file_hash {
		bool operator()(const std::shared_ptr<scene_file>& f) const noexcept {
			return std::hash<std::wstring>{}(f->get_uid());
		}
	};

} // docs_gen_core

#endif // DOCS_GEN_FILE_H
