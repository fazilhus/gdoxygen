#pragma once

#include <filesystem>

namespace core
{

	class parser
	{
		std::filesystem::path path_;

	public:
		parser() = default;
		parser(const parser& other) = delete;
		parser(parser&& other) = delete;
		~parser() = default;

		parser& operator=(const parser& other) = delete;
		parser& operator=(parser&& other) = delete;

		[[nodiscard]] bool set_path(const std::string& path);
		[[nodiscard]] bool is_file() const;
		[[nodiscard]] bool is_dir() const;
	};

} // core
