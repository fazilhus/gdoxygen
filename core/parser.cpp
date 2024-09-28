#include "parser.hpp"

namespace core
{

	bool parser::set_path(const std::string& path)
	{
		const auto temp = std::filesystem::path(path);

		if (!exists(temp)) {
			return false;
		}

		path_ = temp;
		return true;
	}

	bool parser::is_file() const
	{
		return is_regular_file(path_);
	}

	bool parser::is_dir() const
	{
		return is_directory(path_);
	}

} // core
