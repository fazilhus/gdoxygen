#include "file.hpp"

#include <iostream>

namespace docs_gen_core {

	file::file(const std::filesystem::path& path)
		: path_(path) {
		path_.make_preferred();
		title_ = path_.filename().wstring();
	}

	file::file(const file& other)
		: path_(other.path_), title_(other.title_) {
	}

	file::file(file&& other) noexcept
		: path_(std::move(other.path_)), title_(std::move(other.title_)) {
	}

	script_file::script_file(const std::filesystem::path& path)
		: file(path) {
	}

	script_file::script_file(const script_file& other)
		: file(other) {
	}

	script_file::script_file(script_file&& other) noexcept
		: file(std::move(other)) {
	}

	script_file& script_file::operator=(const script_file& other) {
		path_ = other.path_;
		title_ = other.title_;
		return *this;
	}

	script_file& script_file::operator=(script_file&& other) noexcept {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
		return *this;
	}

	ext_resource_other::ext_resource_other(const std::wstring& type, const std::filesystem::path& path)
		: type(type), path(path) {
		name = path.filename().wstring();
	}

	dott_file::dott_file(const std::filesystem::path& path)
		: file(path) {
	}

	dott_file::dott_file(const dott_file& other)
		: file(other), packed_scenes_(other.packed_scenes_), ext_resources_(other.ext_resources_) {}

	dott_file::dott_file(dott_file&& other) noexcept
		: packed_scenes_(std::move(other.packed_scenes_)), ext_resources_(std::move(other.ext_resources_)) {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
	}

	dott_file& dott_file::operator=(const dott_file& other) {
		path_ = other.path_;
		title_ = other.title_;
		packed_scenes_ = other.packed_scenes_;
		ext_resources_ = other.ext_resources_;
		return *this;
	}

	dott_file& dott_file::operator=(dott_file&& other) noexcept {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
		packed_scenes_ = std::move(other.packed_scenes_);
		ext_resources_ = std::move(other.ext_resources_);
		return *this;
	}

	void dott_file::push_packed_scene(const std::wstring& key, const std::shared_ptr<scene_file>& child) {
		if (packed_scenes_.find(key) != packed_scenes_.end()) {
			std::cerr << "[WARNING] overwriting external resource PackedScene ";
			std::wcerr << packed_scenes_[key]->get_path();
			std::cerr << '\n';
		}
		
		packed_scenes_[key] = child;
	}

	void dott_file::push_ext_resource(const std::wstring& key, const std::shared_ptr<resource_file>& resource) {
		if (ext_resources_.find(key) != ext_resources_.end()) {
			std::cerr << "[WARNING] overwriting external resource Resource ";
			std::wcerr << ext_resources_[key];
			std::cerr << '\n';
		}
		
		ext_resources_[key] = resource;
	}

	void dott_file::push_ext_resource_other(const std::wstring& key, const ext_resource_other& resource) {
		if (ext_resources_other_.find(key) != ext_resources_other_.end()) {
			const auto& res = ext_resources_other_[key];
			std::cerr << "[WARNING] overwriting external resource ";
			std::wcerr << res.type << ' ' << res.name;
			std::cerr << '\n';
		}
		
		ext_resources_other_[key] = resource;
	}

	resource_file::resource_file(const std::filesystem::path& path)
		: dott_file(path) {
	}

	resource_file::resource_file(const resource_file& other)
		: dott_file(other), uid_(other.uid_), scripts_(other.scripts_) {
	}

	resource_file::resource_file(resource_file&& other) noexcept
		: uid_(std::move(other.uid_)) {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
		packed_scenes_ = std::move(other.packed_scenes_);
		ext_resources_ = std::move(other.ext_resources_);
		scripts_ = std::move(other.scripts_);
	}

	resource_file& resource_file::operator=(const resource_file& other) {
		path_ = other.path_;
		title_ = other.title_;
		uid_ = other.uid_;
		packed_scenes_ = other.packed_scenes_;
		ext_resources_ = other.ext_resources_;
		scripts_ = other.scripts_;
		return *this;
	}

	resource_file& resource_file::operator=(resource_file&& other) noexcept {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
		packed_scenes_ = std::move(other.packed_scenes_);
		ext_resources_ = std::move(other.ext_resources_);
		scripts_ = std::move(other.scripts_);
		return *this;
	}

	void resource_file::push_script(const std::wstring& key, const std::shared_ptr<script_file>& s) {
		if (scripts_.find(key) != scripts_.end()) {
			std::cerr << "[WARNING] overwriting external resource Script ";
			std::wcerr << scripts_[key]->get_path();
			std::cerr << '\n';
		}
		
		scripts_[key] = s;
	}

	void resource_file::push_sub_resource(const std::wstring& key, const std::shared_ptr<resource>& resource) {
		if (sub_resources_.find(key) != sub_resources_.end()) {
			std::cerr << "[WARNING] overwriting sub_resource ";
			std::wcerr << resource->type;
			std::cerr << '\n';
		}
		
		sub_resources_[key] = resource;
	}

	scene_file::scene_file(const std::filesystem::path& path)
		: dott_file(path) {
	}

	scene_file::scene_file(const scene_file& other)
		: dott_file(other), uid_(other.uid_), scripts_(other.scripts_), node_tree_(other.node_tree_) {
	}

	scene_file::scene_file(scene_file&& other) noexcept
		: uid_(std::move(other.uid_)), node_tree_(std::move(other.node_tree_)) {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
		packed_scenes_ = std::move(other.packed_scenes_);
		ext_resources_ = std::move(other.ext_resources_);
		scripts_ = std::move(other.scripts_);
	}

	scene_file& scene_file::operator=(const scene_file& other) {
		path_ = other.path_;
		title_ = other.title_;
		packed_scenes_ = other.packed_scenes_;
		ext_resources_ = other.ext_resources_;
		scripts_ = other.scripts_;
		uid_ = other.uid_;
		node_tree_ = other.node_tree_;
		return *this;
	}

	scene_file& scene_file::operator=(scene_file&& other) noexcept {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
		packed_scenes_ = std::move(other.packed_scenes_);
		ext_resources_ = std::move(other.ext_resources_);
		scripts_ = std::move(other.scripts_);
		uid_ = std::move(other.uid_);
		node_tree_ = std::move(other.node_tree_);
		return *this;
	}

	void scene_file::push_script(const std::wstring& key, const std::shared_ptr<script_file>& script) {
		if (scripts_.find(key) != scripts_.end()) {
			std::cerr << "[WARNING] overwriting external resource Script ";
			std::wcerr << scripts_[key]->get_path();
			std::cerr << '\n';
		}
		
		scripts_[key] = script;
	}
	
} // docs_gen_core
