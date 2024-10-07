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

	dott_file::dott_file(const std::filesystem::path& path)
		: file(path) {
	}

	dott_file::dott_file(const dott_file& other)
		: file(other), packed_scenes_(other.packed_scenes_), resources_(other.resources_) {}

	dott_file::dott_file(dott_file&& other) noexcept
		: packed_scenes_(std::move(other.packed_scenes_)), resources_(std::move(other.resources_)) {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
	}

	dott_file& dott_file::operator=(const dott_file& other) {
		path_ = other.path_;
		title_ = other.title_;
		packed_scenes_ = other.packed_scenes_;
		resources_ = other.resources_;
		return *this;
	}

	dott_file& dott_file::operator=(dott_file&& other) noexcept {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
		packed_scenes_ = std::move(other.packed_scenes_);
		resources_ = std::move(other.resources_);
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

	void dott_file::push_resource(const std::wstring& key, const std::shared_ptr<resource_file>& resource) {
		if (resources_.find(key) != resources_.end()) {
			std::cerr << "[WARNING] overwriting external resource Script ";
			std::wcerr << resources_[key];
			std::cerr << '\n';
		}
		
		resources_[key] = resource;
	}

	resource_file::resource_file(const std::filesystem::path& path)
		: dott_file(path) {
	}

	resource_file::resource_file(const resource_file& other)
		: dott_file(other), uid_(other.uid_), script_(other.script_) {
	}

	resource_file::resource_file(resource_file&& other) noexcept
		: uid_(std::move(other.uid_)) {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
		packed_scenes_ = std::move(other.packed_scenes_);
		resources_ = std::move(other.resources_);
		script_ = std::move(other.script_);
	}

	resource_file& resource_file::operator=(const resource_file& other) {
		path_ = other.path_;
		title_ = other.title_;
		uid_ = other.uid_;
		packed_scenes_ = other.packed_scenes_;
		resources_ = other.resources_;
		script_ = other.script_;
		return *this;
	}

	resource_file& resource_file::operator=(resource_file&& other) noexcept {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
		packed_scenes_ = std::move(other.packed_scenes_);
		resources_ = std::move(other.resources_);
		script_ = std::move(other.script_);
		return *this;
	}

	scene_file::scene_file(const std::filesystem::path& path)
		: dott_file(path) {
	}

	scene_file::scene_file(const scene_file& other)
		: dott_file(other), uid_(other.uid_), scripts_(other.scripts_) {
	}

	scene_file::scene_file(scene_file&& other) noexcept
		: uid_(std::move(other.uid_)) {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
		packed_scenes_ = std::move(other.packed_scenes_);
		resources_ = std::move(other.resources_);
		scripts_ = std::move(other.scripts_);
	}

	scene_file& scene_file::operator=(const scene_file& other) {
		path_ = other.path_;
		title_ = other.title_;
		uid_ = other.uid_;
		packed_scenes_ = other.packed_scenes_;
		resources_ = other.resources_;
		scripts_ = other.scripts_;
		return *this;
	}

	scene_file& scene_file::operator=(scene_file&& other) noexcept {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
		uid_ = std::move(other.uid_);
		packed_scenes_ = std::move(other.packed_scenes_);
		resources_ = std::move(other.resources_);
		scripts_ = std::move(other.scripts_);
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

	void scene_file::push_sub_resource(const std::wstring& key, const std::wstring& resource_type) {
		if (sub_resources_.find(key) != sub_resources_.end()) {
			std::cerr << "[WARNING] overwriting external resource Script ";
			std::wcerr << sub_resources_[key];
			std::cerr << '\n';
		}
		
		sub_resources_[key] = resource_type;
	}
	
} // docs_gen_core
