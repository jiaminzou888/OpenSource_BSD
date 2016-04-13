#pragma once

template<typename map_t, typename key_t, typename val_t>
typename map_t::iterator efficient_map_operation(map_t& m, const key_t& k, const val_t& v)
{
	typename map_t::iterator map_lb = m.lower_bound(k);

	if (map_lb != m.end() && !(m.key_comp()(k, map_lb->first)))
	{
		map_lb->second = v;
		return map_lb;
	}
	else
	{
		return m.insert(map_lb, map_t::value_type(k, v));
	}
}