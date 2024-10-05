#include "file.hpp"

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

	resource_file::resource_file(const std::filesystem::path& path)
		: file(path) {
	}

	resource_file::resource_file(const resource_file& other)
		: file(other), uid_(other.uid_), script_(other.script_) {
	}

	resource_file::resource_file(resource_file&& other) noexcept
		: uid_(std::move(other.uid_)), script_(std::move(other.script_)) {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
	}

	resource_file& resource_file::operator=(const resource_file& other) {
		path_ = other.path_;
		title_ = other.title_;
		uid_ = other.uid_;
		script_ = other.script_;
		return *this;
	}

	resource_file& resource_file::operator=(resource_file&& other) noexcept {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
		uid_ = std::move(other.uid_);
		script_ = std::move(other.script_);
		return *this;
	}

	void resource_file::set_script(const std::shared_ptr<script_file>& script) {
		script_ = script;
	}

	scene_file::scene_file(const std::filesystem::path& path)
		: file(path) {
	}

	scene_file::scene_file(const scene_file& other)
		: file(other), uid_(other.uid_),
		children_(other.children_), scripts_(other.scripts_) {
	}

	scene_file::scene_file(scene_file&& other) noexcept
		: uid_(std::move(other.uid_)), children_(std::move(other.children_)),
		scripts_(std::move(other.scripts_)) {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
	}

	scene_file& scene_file::operator=(const scene_file& other) {
		path_ = other.path_;
		title_ = other.title_;
		uid_ = other.uid_;
		children_ = other.children_;
		scripts_ = other.scripts_;
		return *this;
	}

	scene_file& scene_file::operator=(scene_file&& other) noexcept {
		path_ = std::move(other.path_);
		title_ = std::move(other.title_);
		uid_ = std::move(other.uid_);
		children_ = std::move(other.children_);
		scripts_ = std::move(other.scripts_);
		return *this;
	}

} // docs_gen_core
