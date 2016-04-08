#pragma once

#include <memory>
#include <vector>
#include <functional>

template <typename T>
class CSimpleObjectPoll
{
public:
	using delete_type = std::function<void(T*)>;

	inline bool empty() const
	{
		return _pool.empty();
	}

	inline size_t size() const
	{
		return _pool.size();
	}

	inline void add(std::unique_ptr<T> ptr_obj)
	{
		_pool.push_back(std::move(ptr_obj));
	}

	inline auto get()->std::unique_ptr<T, delete_type>
	{
		if (empty())
		{
			throw std::logic_error("no more object!");
		}

		auto ret_ptr = std::unique_ptr<T, delete_type>(_pool.back().release(), [this](T* t){_pool.push_back(std::unique_ptr<T>(t)); });
		_pool.pop_back();

		return ret_ptr;
	}

private:
	std::vector<std::unique_ptr<T>> _pool;
};