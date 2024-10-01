#pragma once

#include <filesystem>
#include <vector>

#include <memory>
#include <unordered_map>

#include "file.hpp"

namespace core {

	class dir {
		std::filesystem::path path_;

		std::unordered_map<std::string, std::shared_ptr<scene_file>> scene_files_;
		std::unordered_map<std::string, std::shared_ptr<script_file>> script_files_;

	public:
		dir() = default;
		dir(const dir& other) = delete;
		dir(dir&& other) = delete;
		~dir() = default;

		dir& operator=(const dir& other) = delete;
		dir& operator=(dir&& other) = delete;

		[[nodiscard]] bool set_path(const std::string& path);
		[[nodiscard]] bool is_file() const;
		[[nodiscard]] bool is_dir() const;
	};

} // core
