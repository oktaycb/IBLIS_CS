#pragma once
#include <any>

class Config 
{
public:
	void Init();
	bool Create(const std::string& file);
	bool Save(const std::string& file);
	bool Load(const std::string& file);
	bool Remove(const std::string& file);
	bool Rename(const std::string& old_file, const std::string& new_file);
	bool ExportToClipboard(const std::string& file);
	bool ImportFromClipboard(const std::string& file);
public:
	std::vector<bool>& vb(const std::uint32_t index)
	{
		return m_Items.at(index).get<std::vector<bool>>();
	}

	std::vector<int>& vi(const std::uint32_t index)
	{
		return m_Items.at(index).get<std::vector<int>>();
	}

	std::vector<float>& vf(const std::uint32_t index)
	{
		return m_Items.at(index).get<std::vector<float>>();
	}

	bool& b(const std::uint32_t index)
	{
		return m_Items.at(index).get<bool>();
	}

	int& i(const std::uint32_t index)
	{
		return m_Items.at(index).get<int>();
	}

	float& f(const std::uint32_t index)
	{
		return m_Items.at(index).get<float>();
	}

	Color& c(const std::uint32_t index)
	{
		return m_Items.at(index).get<Color>();
	}

	template<typename t>
	uint32_t AddItem(const hash_t name, const hash_t type, t val) noexcept
	{
		m_Items.emplace_back(name, type, std::make_any<t>(val));
		return m_Items.size() - 1u;
	}

	int32_t DoesItemExist(hash_t name);

	__forceinline std::string GetConfigDirectory()
	{
		return ("iblis\\beta");
	}

	template <typename t, std::size_t s>
	std::vector<t> GetFilledVector(const t& fill)
	{
		std::vector<t> vec(s);
		std::fill(vec.begin(), vec.begin() + s, fill);
		return vec;
	}
private:
	struct Item_t
	{
		Item_t(const hash_t name, const hash_t type, std::any&& val) : m_name(name), m_type(type), m_value(std::move(val)) {}
		~Item_t() = default;

		template<typename t>
		t& get()
		{
			return *static_cast<t*>(std::any_cast<t>(&m_value));
		}

		template<typename t>
		void set(t value)
		{
			m_value.emplace<t>(value);
		}

		hash_t m_name = 0u;
		hash_t m_type = 0u;
		std::any m_value = {};
	};

	std::vector<Item_t> m_Items = { };
};

inline Config g_Config;

#define ADD_CFG(name, type, val) const uint32_t name = g_Config.AddItem<type>(HASH(#name), HASH(#type), val);
#define ADD_VECTOR_CFG( type, size, name, val ) const uint32_t name = g_Config.AddItem<std::vector<type>>(HASH(#name), HASH("std::vector<" #type ">"), g_Config.GetFilledVector<type, size>(val));