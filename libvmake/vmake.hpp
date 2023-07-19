#ifndef _VTEST_VMAKE_HPP
#define _VTEST_VMAKE_HPP

#include <bits/stdc++.h>

#if __cplusplus < 201703L
#include "optional.hpp"
#endif

namespace vmake {

struct require_unique_t {} require_unique;

struct sequence_terminated_error : std::exception {
	virtual const char *what() const noexcept override final {
		return "iterating on a terminated sequence.";
	}
};

namespace polyfill {

template<typename T>
struct as_const_reference_t {
	using type = const typename std::remove_reference<T>::type&;
};

template<typename T>
inline constexpr auto as_const_reference(typename as_const_reference_t<T>::type x) noexcept {
	return x;
}

#if __cplusplus >= 201703L
using std::optional;
#else
using nonstd::optional;
#endif

} // namespace polyfill

namespace details {

template<typename T>
struct iota_sequence {
	using result = T;

	T start;
	iota_sequence(const T& start) : start(start) {}

	bool is_terminated() const noexcept {
		return false;
	}

	auto operator()() {
		return start++;
	}
};

template<typename T>
struct ranged_sequence {
	using result = T;

	T start, end;
	ranged_sequence(const T& start, const T& end) : start(start), end(end) {}

	bool is_terminated() const noexcept {
		return start == end;
	}

	auto operator()() {
		if (start == end) throw sequence_terminated_error();
		return start++;
	}
};

template<typename T>
struct ranged_step_sequence {
	using result = T;

	T start, end, step;
	ranged_step_sequence(const T& start, const T& end, const T& step) : start(start), end(end), step(step) {}

	bool is_terminated() const noexcept {
		return start == end;
	}

	auto operator()() {
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
	return details::ranged_step_sequence<typename std::decay<Val>::type>(std::forward<Val>(start)
		, std::forward<Val>(end), std::forward<Val>(step));
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

	auto operator()() {
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

	auto operator()() {
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
	using result = typename std::result_of<Func()>::type;

	Func g;

	generator(Func &&g) : g(std::forward<Func>(g)) {}
	generator(const Func &g) : g(g) {}

	generator(generator<Func> &&y) = default;
	generator(const generator<Func> &y) = default;

	constexpr bool is_terminated() const noexcept {
		return false;
	}

	auto operator()() {
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

	auto operator()() {
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

template<typename Gen1, typename Gen2>
struct concator {
	using result = typename Gen1::result;
	using core = Gen1;
	using core2 = Gen2;

	Gen1 g1;
	Gen2 g2;

	concator(Gen1 &&g1, Gen2 &&g2) : g1(std::move(g1)), g2(std::move(g2)) {}
	concator(concator<Gen1, Gen2> &&c) = default;
	concator(const concator<Gen1, Gen2> &c) = default;
//	concator<Gen1, Gen2>& operator=(concator<Gen1, Gen2> &&c) = default;

	bool is_terminated() const noexcept {
		return g1.is_terminated() && g2.is_terminated();
	}

	auto operator()() {
		if (g1.is_terminated()) return g2();
		return g1();
	}
};

} // namespace details

template<typename Gen1, typename Gen2>
inline constexpr auto concat(Gen1 &&x, Gen2 &&y) noexcept {
	return details::concator<typename std::decay<Gen1>::type
				, typename std::decay<Gen2>::type>(std::forward<Gen1>(x), std::forward<Gen2>(y));
}

namespace details {

template<typename Gen, typename Func>
struct transformer {
	using result = typename std::result_of<Func(typename Gen::result)>::type;
	using core = Gen;

	Gen g;
	Func f;

	transformer(Gen &&g, Func &&gf) : g(std::move(g)), f(std::move(f)) {}
	transformer(transformer<Gen, Func> &&c) = default;
	transformer(const transformer<Gen, Func> &c) = default;

	bool is_terminated() const noexcept {
		return g.is_terminated();
	}

	auto operator()() {
		return f(g());
	}
};

} // namespace details

template<typename Gen, typename Func>
inline auto transform(Gen &&g, Func &&f) {
	return details::transformer<typename std::decay<Gen>::type
				  , typename std::decay<Func>::type>(std::forward<Gen>(g), std::forward<Func>(f));
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

	filteror(Gen &&g, Pred &&p) : g(std::forward<Gen>(g)), p(std::forward<Pred>(p)), preview() {
		_find_next();
	};

	bool is_terminated() const noexcept {
		if (g.is_terminated() && !preview.has_value()) return true;
		if (!preview.has_value()) _find_next();
		return !preview.has_value();
	}

	auto operator()() {
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
		} while (!p(polyfill::as_const_reference<result>(*preview)));
	}
};

template<typename Gen, typename Pred>
inline filteror<typename std::decay<Gen>::type, typename std::decay<Pred>::type> filter(Gen &&g, Pred &&p) {
	return {std::forward<Gen>(g), std::forward<Pred>(p)};
}

namespace details {

template<typename ...Ts>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));

template<typename Tp, int n>
struct repeat_tuple {
	static_assert(n > 0, "");
	using type = tuple_cat_t<std::tuple<Tp>, typename repeat_tuple<Tp, n - 1>::type>;
};

template<typename Tp>
struct repeat_tuple<Tp, 1> {
	using type = std::tuple<Tp>;
};

template<typename Gen, int n>
struct group_helper {
	static auto load(Gen &g) {
		auto x = g();
		auto y = group_helper<Gen, n - 1>::load(g);
		return std::tuple_cat(std::make_tuple<typename Gen::result>(std::move(x)), std::move(y));
	}
};

template<typename Gen>
struct group_helper<Gen, 1> {
	static auto load(Gen &g) {
		return std::make_tuple<typename Gen::result>(g());
	}
};

template<typename Gen, int n>
struct grouper {
	static_assert(n > 0, "");
	using result = typename repeat_tuple<typename Gen::result, n>::type;
	using core = Gen;

	Gen g;

	grouper(grouper<Gen, n>&& g) = default;
	grouper(const grouper<Gen, n>& g) = default;
	grouper(Gen &&g) : g(std::forward<Gen>(g)) {}

	bool is_terminated() noexcept {
		return g.is_terminated();
	}

	result operator()() {
		return group_helper<Gen, n>::load(g);
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
	std::unordered_set<Tval> used;
	std::vector<int> rest;
	bool halfed;

	unique_ints_sequence(Engine &&e, Tval &&l, Tval &&r) : l(l), r(r), rng(std::forward<Engine>(e))
		, dis(std::forward<Tval>(l), std::forward<Tval>(r)), used(), rest(), halfed(false) {}

	bool is_terminated() const noexcept {
		return halfed && rest.empty();
	}

	auto operator()() {
		if (!halfed && (used.size() + 1) * 2 >= static_cast<_unsigned_Tval>(r - l + 1)) {
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

private:
	using _unsigned_Tval = typename std::conditional<std::is_integral<Tval>::value
		, typename std::make_unsigned<Tval>::type, Tval>::type;
};

};

template<typename Tval = int, typename Engine = std::default_random_engine>
inline auto uniform_ints(require_unique_t _, Tval &&l, Tval &&r) {
	return details::unique_ints_sequence<typename std::decay<Tval>::type, Engine>(Engine(make_seed())
		, std::forward<Tval>(l), std::forward<Tval>(r));
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
	return copy(std::ostream_iterator<typename std::decay<Gen>::type::result>(out, delim), std::move(g));
}

template<typename CharT, typename Traits, typename Gen>
inline auto output(std::basic_ostream<CharT, Traits>& out, Gen &&g) {
	return copy(std::ostream_iterator<typename std::decay<Gen>::type::result>(out), std::move(g));
}

template<typename CharT, typename Traits, typename Gen>
inline auto output_n(std::basic_ostream<CharT, Traits>& out, size_t n, const char *delim, Gen &&g) {
	return copy_n(std::ostream_iterator<typename std::decay<Gen>::type::result>(out, delim), n, std::move(g));
}

template<typename CharT, typename Traits, typename Gen>
inline auto output_n(std::basic_ostream<CharT, Traits>& out, size_t n, Gen &&g) {
	return copy_n(std::ostream_iterator<typename std::decay<Gen>::type::result>(out), n, std::move(g));
}

template<typename CharT, typename Traits, typename Gen, typename Endl = decltype(std::endl<CharT, Traits>)>
inline auto outputln(std::basic_ostream<CharT, Traits>& out, const char *delim, Gen &&g, const Endl &endl = std::endl<CharT, Traits>) {
	auto res = copy(std::ostream_iterator<typename std::decay<Gen>::type::result>(out, delim), std::move(g));
	out << endl;
	return res;
}

template<typename CharT, typename Traits, typename Gen, typename Endl = decltype(std::endl<CharT, Traits>)>
inline auto outputln(std::basic_ostream<CharT, Traits>& out, Gen &&g, const Endl &endl = std::endl<CharT, Traits>) {
	auto res = copy(std::ostream_iterator<typename std::decay<Gen>::type::result>(out), std::move(g));
	out << endl;
	return res;
}

template<typename CharT, typename Traits, typename Gen, typename Endl = decltype(std::endl<CharT, Traits>)>
inline auto outputln_n(std::basic_ostream<CharT, Traits>& out, size_t n
	, const char *delim, Gen &&g, const Endl &endl = std::endl<CharT, Traits>) {
	auto res = copy_n(std::ostream_iterator<typename std::decay<Gen>::type::result>(out, delim), n, std::move(g));
	out << endl;
	return res;
}

template<typename CharT, typename Traits, typename Gen, typename Endl = decltype(std::endl<CharT, Traits>)>
inline auto outputln_n(std::basic_ostream<CharT, Traits>& out, size_t n, Gen &&g, const Endl &endl = std::endl<CharT, Traits>) {
	auto res = copy_n(std::ostream_iterator<typename std::decay<Gen>::type::result>(out), n, std::move(g));
	out << endl;
	return res;
}

template<typename CharT, typename Traits, typename Endl = decltype(std::endl<CharT, Traits>)>
inline void outputln(std::basic_ostream<CharT, Traits>& out, const Endl &endl = std::endl<CharT, Traits>) {
	out << endl;
}

}

#endif
