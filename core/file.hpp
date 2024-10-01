#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <memory>

namespace core {

	class file {
	protected:
		std::filesystem::path path_;
		std::string title_;

		file() = default;
		explicit file(const std::filesystem::path& path);
		file(const file& other);
		file(file&& other) noexcept;
		virtual ~file() = default;

	public:
		[[nodiscard]] const std::filesystem::path& get_path() const { return path_; }
		[[nodiscard]] const std::string& get_title() const { return title_; }
	};

	class script_file final : public file {
	public:
		script_file() = default;
		explicit script_file(const std::filesystem::path& path);
		script_file(const script_file& other);
		script_file(script_file&& other) noexcept;
		~script_file() override = default;

		script_file& operator=(const script_file& other);
		script_file& operator=(script_file&& other) noexcept;
	};

	class resource_file final : public file {
		std::string uid_;
		std::shared_ptr<script_file> script_;

	public:
		resource_file() = default;
		explicit resource_file(const std::filesystem::path& path);
		resource_file(const resource_file& other);
		resource_file(resource_file&& other) noexcept;
		~resource_file() override = default;

		resource_file& operator=(const resource_file& other);
		resource_file& operator=(resource_file&& other) noexcept;

		void set_script(const std::shared_ptr<script_file>& script);

		[[nodiscard]] const std::shared_ptr<script_file>& get_script() const { return script_; }
		[[nodiscard]] std::shared_ptr<script_file>& get_script() { return script_; }
	};

	class scene_file final : public file {
		std::string uid_;
		std::vector<std::shared_ptr<scene_file>> children_;
		std::vector<std::shared_ptr<script_file>> scripts_;

	public:
		scene_file() = default;
		explicit scene_file(const std::filesystem::path& path);
		scene_file(const scene_file& other);
		scene_file(scene_file&& other) noexcept;
		~scene_file() override = default;

		scene_file& operator=(const scene_file& other);
		scene_file& operator=(scene_file&& other) noexcept;

		void set_uid(const std::string& s) { uid_ = s; }
		void push_child(const std::shared_ptr<scene_file>& child) { children_.push_back(child); }
		void push_script(const std::shared_ptr<script_file>& script) { scripts_.push_back(script); }

		[[nodiscard]] const std::string& get_uid() const { return uid_; }
		[[nodiscard]] const std::vector<std::shared_ptr<scene_file>>& get_children() const { return children_; }
		[[nodiscard]] std::vector<std::shared_ptr<scene_file>>& get_children() { return children_; }
		[[nodiscard]] const std::vector <std::shared_ptr<script_file>>& get_scripts() const { return scripts_; }
		[[nodiscard]] std::vector <std::shared_ptr<script_file>>& get_scripts() { return scripts_; }
	};

	struct scene_file_hash {
		bool operator()(const std::shared_ptr<scene_file>& f) const noexcept {
			return std::hash<std::string>{}(f->get_uid());
		}
	};

} // core