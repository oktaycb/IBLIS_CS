#include "../../precompiled.h"

using json = nlohmann::json;

void Config::Init()
{
	std::filesystem::path path = std::filesystem::path(GetConfigDirectory());

	if (!std::filesystem::exists(path))
	{
		std::filesystem::create_directories(path);
	}
}

bool Config::Create(const std::string& file)
{
	std::filesystem::path path = std::filesystem::path(GetConfigDirectory()) /= file;

	if (!std::filesystem::exists(path))
	{
		std::ofstream file_out(path);
		file_out.close();
	}

	return true;
}

bool Config::Save(const std::string& file)
{
	std::filesystem::path path = std::filesystem::path(GetConfigDirectory()) /= file;

	if (!std::filesystem::exists(path)) // Shouldn't happen
		return false;

	json out;

	try
	{
		for (auto& item : m_Items)
		{
			json j;

			j[XOR("name")] = item.m_name;
			j[XOR("type")] = item.m_type;

			switch (item.m_type)
			{
			case HASH("bool"):
			{
				j[XOR("value")] = item.get<bool>();
			}break;

			case HASH("int"):
			{
				j[XOR("value")] = item.get<int>();
			}break;

			case HASH("float"):
			{
				j[XOR("value")] = item.get<float>();
			}break;

			case HASH("std::string"):
			{
				j[XOR("value")] = item.get<std::string>();
			}break;

			case HASH("Color"):
			{
				Color color = item.get<Color>();
				json sub = {};

				sub.push_back(color.r());
				sub.push_back(color.g());
				sub.push_back(color.b());
				sub.push_back(color.a());

				j[XOR("value")] = sub.dump();
			}break;

			case HASH("std::vector<bool>"):
			{
				const auto& vec = item.get<std::vector<bool>>();
				json sub = {};

				// fill node with all vector values
				for (const auto&& val : vec)
					sub.push_back(static_cast<bool>(val));

				j[XOR("value")] = sub.dump();
			}break;
			case HASH("std::vector<int>"):
			{
				const auto& vec = item.get<std::vector<int>>();
				json sub = {};

				// fill node with all vector values
				for (const auto& val : vec)
					sub.push_back(val);

				j[XOR("value")] = sub.dump();
			}break;
			case HASH("std::vector<float>"):
			{
				const auto& vec = item.get<std::vector<float>>();

				// store vector values as sub-node
				json sub = {};

				// fill node with all vector values
				for (const auto& val : vec)
					sub.push_back(val);

				j[XOR("value")] = sub.dump();
			}break;

			default:
				break;
			}

			out.push_back(j);
		}
	}
	catch (const nlohmann::detail::exception& ex)
	{
		MessageBoxA(nullptr, ex.what(), nullptr, 0);
		return false;
	}

	std::ofstream file_out(path, std::ios::out | std::ios::trunc);

	if (!file_out.good())
		return false;
	try 
	{
		file_out << out.dump(1);
		file_out.close();
		out.clear();
	}
	catch (std::ofstream::failure& ex)
	{
		MessageBoxA(nullptr, ex.what(), nullptr, 0);
		return false;
	}

	return true;
}

bool Config::Load(const std::string& file)
{
	std::filesystem::path path = std::filesystem::path(GetConfigDirectory()) /= file;

	if (!std::filesystem::exists(path))
		return false;

	nlohmann::json in = {};

	std::ifstream config_file(path, std::ios::in);

	if (!config_file.good())
		return false;

	try
	{
		// parse saved variables
		in = nlohmann::json::parse(config_file, nullptr, false);

		// check is json parse failed
		if (in.is_discarded())
			return false;

		config_file.close();
	}
	catch (std::ifstream::failure& ex)
	{
		MessageBoxA(nullptr, ex.what(), nullptr, 0);
		return false;
	}

	try {
		for (const auto& item : in)
		{
			const hash_t item_index = DoesItemExist(item[("name")].get<hash_t>());
			if (item_index == -1)
				continue;

			auto& current_item = m_Items.at(item_index);

			switch (item[XOR("type")].get<hash_t>())
			{

			case HASH("bool"):
			{
				current_item.set<bool>(item[XOR("value")].get<bool>());
			}break;

			case HASH("int"):
			{
				current_item.set<int>(item[XOR("value")].get<int>());
			}break;

			case HASH("float"):
			{
				current_item.set<float>(item[XOR("value")].get<float>());
			}break;

			case HASH("std::string"):
			{
				current_item.set<std::string>(item[XOR("value")].get<std::string>());
			}break;

			case HASH("Color"):
			{
				auto vec = json::parse(item[XOR("value")].get<std::string>());

				current_item.set<Color>(Color(
					vec.at(0).get<int>(),
					vec.at(1).get<int>(),
					vec.at(2).get<int>(),
					vec.at(3).get<int>()));
			}break;

			case HASH("std::vector<bool>"):
			{
				const json vector = json::parse(item[XOR("value")].get<std::string>());
				auto& vec = current_item.get<std::vector<bool>>();

				for (std::size_t i = 0U; i < vector.size(); i++)
				{
					// check is item out of bounds
					if (i < vec.size())
						vec.at(i) = vector.at(i).get<bool>();
				}

			}break;

			case HASH("std::vector<int>"):
			{
				const json vector = json::parse(item[XOR("value")].get<std::string>());;
				auto& vec = current_item.get<std::vector<int>>();

				for (std::size_t i = 0U; i < vector.size(); i++)
				{
					// check is item out of bounds
					if (i < vec.size())
						vec.at(i) = vector.at(i).get<int>();
				}

			}break;

			case HASH("std::vector<float>"):
			{
				const json vector = json::parse(item[XOR("value")].get<std::string>());
				auto& vec = current_item.get<std::vector<float>>();

				for (std::size_t i = 0U; i < vector.size(); i++)
				{
					// check is item out of bounds
					if (i < vec.size())
						vec.at(i) = vector.at(i).get<float>();
				}

			}break;

			default:
				break;
			}
		}
	}
	catch (const nlohmann::detail::exception& ex) {

		MessageBoxA(nullptr, ex.what(), nullptr, 0);
		return false;
	}

	return true;
}

bool Config::Remove(const std::string& file)
{
	std::filesystem::path path = std::filesystem::path(GetConfigDirectory()) /= file;

	if (!std::filesystem::exists(path))
		return false;

	std::filesystem::remove(path);
	return true;
}

bool Config::Rename(const std::string& old_file, const std::string& new_file)
{
	std::filesystem::path path = std::filesystem::path(GetConfigDirectory()) /= old_file;

	if (!std::filesystem::exists(path))
		return false;

	std::filesystem::rename(path.string(), path.parent_path() /= new_file);
	return true;
}

bool Config::ExportToClipboard(const std::string& file)
{
	std::filesystem::path path = std::filesystem::path(GetConfigDirectory()) /= file;

	if (!std::filesystem::exists(path))
		return false;

	std::ifstream file_in(path);

	if (!file_in.good())
		return false;

	std::string buffer = std::string(std::istreambuf_iterator<char>(file_in), std::istreambuf_iterator<char>());

	file_in.close(); // No need to read the file anymore

	if (!OpenClipboard(nullptr))
		return false;

	EmptyClipboard();

	const HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, buffer.size() + 1);

	if (!hg)
	{
		CloseClipboard();
		return false;
	}

	std::memcpy(GlobalLock(hg), buffer.data(), buffer.size() + 1);

	GlobalUnlock(hg);

	SetClipboardData(CF_TEXT, hg);

	CloseClipboard();

	return true;
}

bool Config::ImportFromClipboard(const std::string& file)
{
	std::filesystem::path path = std::filesystem::path(GetConfigDirectory()) /= file;

	if (!OpenClipboard(nullptr))
		return false;

	const HANDLE clipboard_data = GetClipboardData(CF_TEXT);

	std::string_view clipboard_text = static_cast<char*>(GlobalLock(clipboard_data));

	GlobalUnlock(clipboard_data);

	std::ofstream file_out(path, std::ios::trunc);
	file_out << clipboard_text;
	file_out.close();

	CloseClipboard();

	if (!Load(file))
		return false;

	return true;
}

int32_t Config::DoesItemExist(hash_t name)
{
	for (size_t i = 0u; i < m_Items.size(); i++)
	{
		if (m_Items.at(i).m_name == name)
			return i;
	}
	return -1;
}