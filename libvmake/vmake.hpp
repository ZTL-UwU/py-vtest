#ifndef _VTEST_VMAKE_HPP
#define _VTEST_VMAKE_HPP

#include <bits/stdc++.h>

#if __cplusplus < 201703L
#include "optional.hpp"
#endif

namespace vmake {

using std::size_t;

struct require_unique_t {} require_unique;

struct sequence_terminated_error : std::exception {
	virtual const char *what() const noexcept override final {
		return "iterating on a terminated sequence.";
	}
};

namespace polyfill {

#if __cplusplus >= 201703L

using std::optional;
using std::as_const;
using std::apply;

#else

using nonstd::optional;

template<typename T> struct add_const_t { using type = const T; };
template<typename T> constexpr typename add_const_t<T>::type& as_const(T& t) noexcept { return t; }
template<typename T> void as_const(const T&&) = delete;

namespace details {

template<typename Func, typename Tuple, size_t ...index>
decltype(auto) apply_helper(Func &&f, Tuple &&t, std::index_sequence<index...>) {
	return f(std::get<index>(std::forward<Tuple>(t))...);
}

}

template<typename Func, typename Tuple>
decltype(auto) apply(Func &&f, Tuple &&t) {
	return details::apply_helper(std::forward<Func>(f), std::forward<Tuple>(t)
		, std::make_index_sequence<std::tuple_size<
		typename std::decay<Tuple>::type>::value>());
}

#endif

} // namespace polyfill

namespace details {

template<typename T>
auto is_sequence_helper(char) -> decltype(
	std::declval<T>()()
	, typename std::enable_if<std::is_same<decltype(
		std::declval<typename polyfill::add_const_t<T>::type>().is_terminated())
		, bool>::value, int>::type{0}
	, typename std::enable_if<std::is_same<decltype(std::declval<T>()())
		, typename T::result>::value, int>::type{0}
	, std::true_type{});

template<typename T>
auto is_sequence_helper(int) -> std::false_type;

}

template<typename T>
using is_sequence_t = decltype(details::is_sequence_helper<T>(' '));

namespace details {

template<typename T>
struct iota_sequence {
	using result = T;

	T start;
	iota_sequence(T&& start) : start(std::forward<T>(start)) {}

	bool is_terminated() const noexcept {
		return false;
	}

	decltype(auto) operator()() {
		return start++;
	}
};

template<typename T>
struct ranged_sequence {
	using result = T;

	T start, end;
	ranged_sequence(T&& start, T&& end)
		: start(std::forward<T>(start)), end(std::forward<T>(end)) {}

	bool is_terminated() const noexcept {
		return !(start < end);
	}

	decltype(auto) operator()() {
		if (start == end) throw sequence_terminated_error();
		return start++;
	}
};

template<typename T>
struct ranged_step_sequence {
	using result = T;

	T start, end, step;
	ranged_step_sequence(T&& start, T&& end, T&& step)
		: start(std::forward<T>(start)), end(std::forward<T>(end))
		, step(std::forward<T>(step)) {}

	bool is_terminated() const noexcept {
		return !(start < end);
	}

	decltype(auto) operator()() {
		if (start == end) throw sequence_terminated_error();
		T res = start;
		start += step;
		return res;
	}
};

} // namespace details

template<typename Val>
inline auto iota(Val &&start) {
	return details::iota_sequence<typename std::decay<Val>::type>(std::forward<Val>(start));
}

template<typename Val>
inline auto range(Val &&start, Val &&end) {
	return details::ranged_sequence<typename std::decay<Val>::type>(std::forward<Val>(start)
		, std::forward<Val>(end));
}

template<typename Val>
inline auto range(Val &&start, Val &&end, Val &&step) {
	return details::ranged_step_sequence<typename std::decay<Val>::type>(
		std::forward<Val>(start), std::forward<Val>(end), std::forward<Val>(step));
}

namespace details {

template<typename T>
struct empty_sequence {
	using result = T;

	bool is_terminated() const noexcept {
		return true;
	}

	T operator()() {
		throw sequence_terminated_error();
	}
};

} // namespace details

template<typename T>
inline auto nothing() {
	return details::empty_sequence<T>();
}

namespace details {

template<typename ContIt>
struct ranged_iterator_extractor {
	using result = typename std::remove_reference<decltype(*std::declval<ContIt>())>::type;

	ContIt cur, end;
	bool added;

	ranged_iterator_extractor(const ContIt &begin, const ContIt &end) : cur(begin)
		, end(end), added(false) {}

	bool is_terminated() const noexcept {
		return cur == end;
	}

	decltype(auto) operator()() {
		// WARN: use for strange iterators such as std::istream_iterator
		// do NOT try to optimize this into *it++
		if (cur == end) throw sequence_terminated_error();
		if (added) return *++cur;
		added = true;
		return *cur;
	}
};

template<typename ContIt>
struct iterator_extractor {
	using result = typename std::remove_reference<decltype(*std::declval<ContIt>())>::type;

	ContIt it;
	bool added;

	iterator_extractor(const ContIt &begin) : it(begin), added(false) {};

	constexpr bool is_terminated() const noexcept {
		return false;
	}

	decltype(auto) operator()() {
		// WARN: use for strange iterators such as std::istream_iterator
		// do NOT try to optimize this into *it++
		if (added) return *++it;
		added = true;
		return *it;
	}
};

} // namespace details

template<typename ContIt>
inline auto extract(const ContIt &begin, const ContIt &end) {
	return details::ranged_iterator_extractor<ContIt>(begin, end);
}

template<typename ContIt>
inline auto extract(const ContIt &begin) {
	return details::iterator_extractor<ContIt>(begin);
}

template<typename ContIt>
inline auto extract_n(const ContIt &begin, size_t n) {
	return take(details::iterator_extractor<ContIt>(begin), n);
}

namespace details {

template<typename Func>
struct generator {
	using result = decltype(std::declval<Func>()());

	Func g;

	generator(Func &&g) : g(std::forward<Func>(g)) {}
	generator(const Func &g) : g(g) {}

	generator(generator<Func> &&y) = default;
	generator(const generator<Func> &y) = default;

	constexpr bool is_terminated() const noexcept {
		return false;
	}

	decltype(auto) operator()() {
		return g();
	}
};

} // namespace details

template<typename Func, typename... Args>
inline auto make_generator(Args&&... args) {
	return details::generator<Func>(std::move(Func(std::forward<Args>(args)...)));
}

template<typename Func>
inline constexpr auto generate(Func &&f) {
	return details::generator<typename std::decay<Func>::type>(std::forward<Func>(f));
}

namespace details {

template<typename Gen>
struct limitor {
	using core = Gen;
	using result = typename Gen::result;

	Gen g;
	size_t lim;
	limitor(Gen &&g, size_t lim) : g(std::move(g)), lim(lim) {}
	limitor(limitor<Gen> &&g) = default;
	limitor(const limitor<Gen> &g) = default;

	bool is_terminated() const noexcept {
		return lim <= 0 || g.is_terminated();
	}

	decltype(auto) operator()() {
		if (lim-- > 0) return g();
		else throw sequence_terminated_error();
	}
};

} // namespace details

template<typename Gen>
inline auto take(Gen &&g, size_t lim) {
	return details::limitor<typename std::decay<decltype(g)>::type>(std::forward<Gen>(g), lim);
};

namespace details {

template<typename Tval>
struct repeater {
	using result = Tval;

	Tval x;
	repeater(Tval &&x) : x(std::forward<Tval>(x)) {};
	repeater(const repeater<Tval>&) = default;
	repeater(repeater<Tval> &&) = default;

	constexpr bool is_terminated() const noexcept {
		return false;
	}

	Tval operator()() const {
		return x;
	}
};

} // namespace details

template<typename Tval>
inline auto repeat(Tval &&x) {
	return details::repeater<typename std::decay<Tval>::type>(std::forward<Tval>(x));
}

template<typename Tval>
inline auto repeat_n(Tval &&x, size_t n) {
	return take(repeat(std::forward<Tval>(x)), n);
}

namespace details {

inline constexpr bool concat_terminate_helper() noexcept {
	return true;
}

template<typename Seq, typename ...Seqs>
inline bool concat_terminate_helper(Seq& f, Seqs& ...rest) noexcept {
	if (!f.is_terminated()) return false;
	return concat_terminate_helper(rest...);
}

template<typename Tuple, size_t ...index>
inline auto concat_terminate_applier(Tuple &t, std::index_sequence<index...>) noexcept {
	return polyfill::apply(concat_terminate_helper<
		typename std::tuple_element<index, Tuple>::type...>, t);
}

template<typename Seq, typename ...Seqs>
inline decltype(auto) concat_call_helper(Seq& f, Seqs& ...rest) {
	if (f.is_terminated()) return concat_call_helper(rest...);
	return f();
}

template<typename Seq>
inline decltype(auto) concat_call_helper(Seq &f) {
	return f();
}

template<typename Tuple, size_t ...index>
inline decltype(auto) concat_call_applier(Tuple &t, std::index_sequence<index...>) {
	return polyfill::apply(concat_call_helper<
		typename std::tuple_element<index, Tuple>::type...>, t);
}

template<typename Gen1, typename ...Gen>
struct concator {
	using result = typename Gen1::result;
	using core = std::tuple<Gen1, Gen...>;

	core g;

	concator(Gen1&& g1, Gen&&... g2) : g(std::forward<Gen1>(g1), std::forward<Gen>(g2)...) {}
	concator(concator<Gen1, Gen...>&&) = default;
	concator(const concator<Gen1, Gen...>&) = default;

	bool is_terminated() const noexcept {
		return concat_terminate_applier(g, std::make_index_sequence<sizeof...(Gen) + 1>());
	}

	decltype(auto) operator()() {
		return concat_call_applier(g, std::make_index_sequence<sizeof...(Gen) + 1>());
	}
};

} // namespace details

template<typename ...Gen>
inline constexpr auto concat(Gen&&... g) noexcept {
	return details::concator<typename std::decay<Gen>::type...>(std::forward<Gen>(g)...);
}

namespace details {

template<typename Gen, typename Func>
struct transformer {
	using result = decltype(std::declval<Func>()(std::declval<typename Gen::result>()));
	using core = Gen;

	Gen g;
	Func f;

	transformer(Gen &&g, Func &&gf) : g(std::move(g)), f(std::move(f)) {}
	transformer(transformer<Gen, Func> &&c) = default;
	transformer(const transformer<Gen, Func> &c) = default;

	bool is_terminated() const noexcept {
		return g.is_terminated();
	}

	decltype(auto) operator()() {
		return f(g());
	}
};

} // namespace details

template<typename Gen, typename Func>
inline auto transform(Gen &&g, Func &&f) {
	return details::transformer<typename std::decay<Gen>::type
				  , typename std::decay<Func>::type>(std::forward<Gen>(g)
				  , std::forward<Func>(f));
}

template<typename Gen, typename Pred>
struct filteror {
	using result = typename Gen::result;
	using core = Gen;

	filteror(filteror<Gen, Pred> &&) = default;
	filteror(const filteror<Gen, Pred> &) = default;

	mutable Gen g;
	Pred p;
	mutable polyfill::optional<result> preview;

	filteror(Gen &&g, Pred &&p) : g(std::forward<Gen>(g)), p(std::forward<Pred>(p)) {
		_find_next();
	};

	bool is_terminated() const noexcept {
		if (g.is_terminated() && !preview.has_value()) return true;
		if (!preview.has_value()) _find_next();
		return !preview.has_value();
	}

	decltype(auto) operator()() {
		if (is_terminated()) throw sequence_terminated_error();
		result res = std::move(*preview);
		preview.reset();
		return res;
	}

private:
	void _find_next() const {
		preview.reset();
		do {
			if (g.is_terminated()) {
				preview.reset();
				return;
			}
			preview.emplace(std::move(g()));
		} while (!p(polyfill::as_const<result>(*preview)));
	}
};

template<typename Gen, typename Pred>
inline filteror<typename std::decay<Gen>::type
	, typename std::decay<Pred>::type> filter(Gen &&g, Pred &&p) {
	return {std::forward<Gen>(g), std::forward<Pred>(p)};
}

namespace details {

//template<typename ...Ts>
//using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));

template<typename Tp, int n, size_t ...index>
auto repeat_tuple_builder(std::index_sequence<index...> seq)
	-> std::tuple<typename std::enable_if<(void(index), true), Tp>::type...>;

template<typename Tp, int n>
struct repeat_tuple_t {
	using type = decltype(repeat_tuple_builder<Tp, n>(std::make_index_sequence<n>()));
};

template<typename Gen, size_t ...index>
inline auto group_builder(Gen &g, std::index_sequence<index...> seq) {
	auto val = std::array<typename Gen::result, seq.size()>{
		(void(index), g())...
	};
	return std::make_tuple(std::move(val[index])...);
}

template<typename Gen, int n>
struct grouper {
	static_assert(n > 0, "");
	using result = typename repeat_tuple_t<typename Gen::result, n>::type;
	using core = Gen;

	Gen g;

	grouper(grouper<Gen, n>&& g) = default;
	grouper(const grouper<Gen, n>& g) = default;
	grouper(Gen &&g) : g(std::forward<Gen>(g)) {}

	bool is_terminated() noexcept {
		return g.is_terminated();
	}

	result operator()() {
		return group_builder<Gen>(g, std::make_index_sequence<n>());
	}
};

}

template<int n, typename Gen>
inline auto group(Gen &&g) {
	return details::grouper<typename std::decay<Gen>::type, n>(std::forward<Gen>(g));
}

namespace rng {

inline auto make_seed() {
	// NOTE: MinGW GCC older than 9.2 have a fixed random_device
#if defined(__GNUC__) && defined(_WIN32) && __GNUC__ * 100 + __GNUC_MINOR__ <= 902
	return std::chrono::steady_clock::now().time_since_epoch().count();
#else
	return std::random_device{}();
#endif
}

template<typename Engine = std::default_random_engine>
inline auto common() {
	return make_generator<Engine>(make_seed());
}

template<typename Engine, typename Seed>
inline auto common(Seed seed) {
	return make_generator<Engine>(seed);
}

namespace details {

struct cstyle_rng {
	using result = decltype(std::rand());

	bool is_terminated() const noexcept {
		return false;
	}

	auto operator()() {
		return std::rand();
	}
};

}

inline auto cstyle() {
	std::srand(time(0));
	std::rand();
	return details::cstyle_rng();
}

inline auto cstyle(unsigned seed) {
	std::srand(seed);
	std::rand();
	return details::cstyle_rng();
}

template<typename Tval = int, typename Engine = std::default_random_engine>
inline auto uniform_ints(Tval &&l, Tval &&r) {
	return generate([rng = Engine(make_seed())
		, dis = std::uniform_int_distribution<typename std::decay<Tval>::type>(
			std::forward<Tval>(l), std::forward<Tval>(r))]() mutable {
		return dis(rng);
	});
}

namespace details {

template<typename Tval, typename Engine>
struct unique_ints_sequence {
	using result = Tval;

	unique_ints_sequence(unique_ints_sequence<Tval, Engine> &&) = default;
	unique_ints_sequence(const unique_ints_sequence<Tval, Engine> &) = default;

	Tval l, r;
	Engine rng;
	std::uniform_int_distribution<Tval> dis;
	bool halfed;
	std::unordered_set<Tval> used;
	std::vector<int> rest;

	unique_ints_sequence(Engine &&e, Tval &&l, Tval &&r) : l(std::forward<Tval>(l))
		, r(std::forward<Tval>(r)), rng(std::forward<Engine>(e))
		, dis(std::forward<Tval>(l), std::forward<Tval>(r)), halfed(false) {}

	bool is_terminated() const noexcept {
		return halfed && rest.empty();
	}

	auto operator()() {
		if (!halfed && (used.size() + 1) * 2 - (r - l + 1) >= 0) {
			for (Tval i = l; i <= r; ++i) {
				if (!used.count(i)) rest.push_back(i);
			}
			std::shuffle(rest.begin(), rest.end(), rng);
			halfed = true;
		}

		if (halfed) {
			if (rest.empty()) throw sequence_terminated_error();
			auto res = std::move(rest.back());
			rest.pop_back();
			return res;
		}

		while (true) {
			auto res = dis(rng);
			if (used.count(res)) continue;
			used.insert(res);
			return res;
		}
	}
};

};

template<typename Tval = int, typename Engine = std::default_random_engine>
inline auto uniform_ints(require_unique_t _, Tval &&l, Tval &&r) {
	return details::unique_ints_sequence<typename std::decay<Tval>::type, Engine>(
		Engine(make_seed()), std::forward<Tval>(l), std::forward<Tval>(r));
}

template<typename Tval = double, typename Engine = std::default_random_engine>
inline auto uniform_reals(Tval &&l, Tval &&r) {
	return generate([rng = Engine(make_seed())
		, dis = std::uniform_real_distribution<typename std::decay<Tval>::type>(
			std::forward<Tval>(l), std::forward<Tval>(r))]() mutable {
		return dis(rng);
	});
}

}

template<typename OutputIt, typename Gen>
inline OutputIt copy(OutputIt it, Gen&& g) {
	while (!g.is_terminated()) {
		*it++ = g();
	}
	return it;
}

template<typename OutputIt, typename Gen>
inline OutputIt copy_n(OutputIt it, size_t n, Gen&& g) {
	for (size_t i = 0; i < n; ++i) {
		*it++ = g();
	}
	return it;
}

template<typename CharT, typename Traits, typename Gen>
inline auto output(std::basic_ostream<CharT, Traits>& out, const char *delim, Gen &&g) {
	return copy(std::ostream_iterator<
		typename std::decay<Gen>::type::result>(out, delim), std::move(g));
}

template<typename CharT, typename Traits, typename Gen>
inline auto output(std::basic_ostream<CharT, Traits>& out, Gen &&g) {
	return copy(std::ostream_iterator<
		typename std::decay<Gen>::type::result>(out), std::move(g));
}

template<typename CharT, typename Traits, typename Gen>
inline auto output_n(std::basic_ostream<CharT, Traits>& out, size_t n, const char *delim, Gen &&g) {
	return copy_n(std::ostream_iterator<
		typename std::decay<Gen>::type::result>(out, delim), n, std::move(g));
}

template<typename CharT, typename Traits, typename Gen>
inline auto output_n(std::basic_ostream<CharT, Traits>& out, size_t n, Gen &&g) {
	return copy_n(std::ostream_iterator<
		typename std::decay<Gen>::type::result>(out), n, std::move(g));
}

template<typename CharT, typename Traits, typename Gen
	, typename Endl = decltype(std::endl<CharT, Traits>)>
inline auto outputln(std::basic_ostream<CharT, Traits>& out, const char *delim
	, Gen &&g, const Endl &endl = std::endl<CharT, Traits>) {
	auto res = copy(std::ostream_iterator<
		typename std::decay<Gen>::type::result>(out, delim), std::forward<Gen>(g));
	out << endl;
	return res;
}

template<typename CharT, typename Traits, typename Gen
	, typename Endl = decltype(std::endl<CharT, Traits>)>
inline auto outputln(std::basic_ostream<CharT, Traits>& out
	, Gen &&g, const Endl &endl = std::endl<CharT, Traits>) {
	auto res = copy(std::ostream_iterator<
		typename std::decay<Gen>::type::result>(out), std::forward<Gen>(g));
	out << endl;
	return res;
}

template<typename CharT, typename Traits, typename Gen
	, typename Endl = decltype(std::endl<CharT, Traits>)>
inline auto outputln_n(std::basic_ostream<CharT, Traits>& out, size_t n
	, const char *delim, Gen &&g, const Endl &endl = std::endl<CharT, Traits>) {
	auto res = copy_n(std::ostream_iterator<
		typename std::decay<Gen>::type::result>(out, delim), n, std::forward<Gen>(g));
	out << endl;
	return res;
}

template<typename CharT, typename Traits, typename Gen
	, typename Endl = decltype(std::endl<CharT, Traits>)>
inline auto outputln_n(std::basic_ostream<CharT, Traits>& out, size_t n
	, Gen &&g, const Endl &endl = std::endl<CharT, Traits>) {
	auto res = copy_n(std::ostream_iterator<
		typename std::decay<Gen>::type::result>(out), n, std::forward<Gen>(g));
	out << endl;
	return res;
}

template<typename CharT, typename Traits, typename Endl = decltype(std::endl<CharT, Traits>)>
inline void outputln(std::basic_ostream<CharT, Traits>& out
	, const Endl &endl = std::endl<CharT, Traits>) {
	out << endl;
}

namespace _checks {

using empty_sequence_int = decltype(nothing<int>());

/*
static_assert(is_sequence_t<empty_sequence_int>::value
	&& is_sequence_t<decltype(take(empty_sequence_int{}, 1))>::value
	&& is_sequence_t<decltype(group<20>(empty_sequence_int{}))>::value
	&& is_sequence_t<decltype(concat(empty_sequence_int{}, empty_sequence_int{}))>::value
	&& !is_sequence_t<int>::value
	&& !is_sequence_t<std::less<int>>::value
	, "compile-time self-checking failed(try upgrading your compiler).");
*/
} // namespace _checks

} // namespace vmake

#endif
