#ifndef _VTEST_VMAKE_HPP
#define _VTEST_VMAKE_HPP

#include <bits/stdc++.h>
#include "optional.hpp"

namespace vmake {

struct sequence_terminated_error : std::exception {
	// TODO
};

namespace details {

template<typename ContIt>
struct extractor {
	using core = ContIt;
	using result = typename std::remove_reference<decltype(*std::declval<ContIt>())>::type;

	ContIt cur, end;

	extractor(const ContIt &begin, const ContIt &end) : cur(begin), end(end) {}

	bool is_terminated() const {
		return cur == end;
	}

	auto operator()() {
		if (cur == end) throw sequence_terminated_error();
		return *cur++;
	}
};

template<typename Func>
struct generator {
	using core = Func;
	using result = typename std::result_of<Func()>::type;

	Func g;

	generator(Func &&g) : g(g) {}
	generator(const Func &g) : g(g) {}

	generator<Func>& operator=(generator<Func> &&y) = default;
	generator<Func>& operator=(const generator<Func> &y) = default;
	generator(generator<Func> &&y) = default;
	generator(const generator<Func> &y) = default;

	bool is_terminated() const {
		return false;
	}

	auto operator()() {
		return g();
	}
};

template<typename Gen>
struct limitor {
	using core = Gen;
	using result = typename Gen::result;

	Gen g;
	size_t lim;
	limitor(Gen &&g, size_t lim) : g(std::move(g)), lim(lim) {}
	limitor(limitor<Gen> &&g) = default;
	limitor(const limitor<Gen> &g) = default;

	bool is_terminated() const {
		return lim <= 0;
	}

	auto operator()() {
		if (lim-- > 0) return g();
		else throw sequence_terminated_error();
	}
};

}

template<typename Gen>
inline auto take(Gen &&g, size_t lim) {
	return details::limitor<typename std::decay<decltype(g)>::type>(std::move(g), lim);
};

template<typename Func, typename... Args>
inline auto make_generator(Args ...args) {
	return details::generator<Func>(std::move(Func(args...)));
}

namespace rng {

template<typename Engine>
inline auto common() {
	// NOTE: MinGW GCC older than 9.2 have a fixed random_device
	return make_generator<Engine>(std::random_device{}());
}

template<typename Engine, typename Seed>
inline auto common(Seed seed) {
	return make_generator<Engine>(seed);
}

struct cstyle_rng {
	using result = int;

	bool is_terminated() const {
		return false;
	}

	auto operator()() {
		return std::rand();
	}
};

inline auto cstyle() {
	std::srand(time(0));
	std::rand();
	return cstyle_rng();
}

inline auto cstyle(int seed) {
	std::srand(seed);
	std::rand();
	return cstyle_rng();
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

template<typename ContIt>
inline auto extract(const ContIt &begin, const ContIt &end) {
	return details::extractor<ContIt>(begin, end);
}

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

	bool is_terminated() const {
		return g1.is_terminated() && g2.is_terminated();
	}

	auto operator()() {
		if (g1.is_terminated()) return g2();
		return g1();
	}
};

template<typename Gen1, typename Gen2>
inline auto concat(Gen1 &&x, Gen2 &&y) {
	return concator<Gen1, Gen2>(std::move(x), std::move(y));
}

template<typename Gen, typename Func>
struct transformer {
	using result = typename std::result_of<Func(typename Gen::result)>::type;
	using core = Gen;

	Gen g;
	Func f;

	transformer(Gen &&g, Func &&gf) : g(std::move(g)), f(std::move(f)) {}
	transformer(transformer<Gen, Func> &&c) = default;
	transformer(const transformer<Gen, Func> &c) = default;
//	transformer& operator=(transformer<Gen, Func> &&c) = default;

	bool is_terminated() const {
		return g.is_terminated();
	}

	auto operator()() {
		return f(g());
	}
};

template<typename Gen, typename Func>
inline auto transform(Gen &&g, Func &&f) {
	return transformer<Gen, Func>(std::move(g), std::move(f));
}

template<typename OutputStream, typename Gen>
inline auto output(OutputStream& out, const char *delim, Gen &&g) {
	return copy(std::ostream_iterator<typename std::decay<Gen>::type::result>(out, delim), std::move(g));
}

template<typename OutputStream, typename Gen>
inline auto output(OutputStream& out, Gen &&g) {
	return copy(std::ostream_iterator<typename std::decay<Gen>::type::result>(out), g);
}

template<typename OutputStream, typename Gen>
inline auto output_n(OutputStream& out, const char *delim, size_t n, Gen &&g) {
	return copy_n(std::ostream_iterator<typename std::decay<Gen>::type::result>(out, delim), n, std::move(g));
}

template<typename OutputStream, typename Gen>
inline auto output_n(OutputStream& out, size_t n, Gen &&g) {
	return copy_n(std::ostream_iterator<typename std::decay<Gen>::type::result>(out), n, std::move(g));
}

template<typename Func>
inline auto generate(Func &&f) {
	return details::generator<Func>(std::move(f));
}

namespace polyfill {

template<typename T>
struct as_const_reference_t {
	using type = const typename std::remove_reference<T>::type&;
};

template<typename T>
inline auto as_const_reference(typename as_const_reference_t<T>::type x) {
	return x;
}

template<typename T>
using optional = nonstd::optional<T>;

}

template<typename Gen, typename Pred>
struct filteror {
	using result = typename Gen::result;
	using core = Gen;

	filteror(filteror<Gen, Pred> &&) = default;
	filteror(const filteror<Gen, Pred> &) = default;

	Gen g;
	Pred p;
	polyfill::optional<result> preview;

	filteror(Gen &&g, Pred &&p) : g(std::forward<Gen>(g)), p(std::forward<Pred>(p)), preview() {
		_find_next();
	};

	bool is_terminated() const {
		return g.is_terminated() && !preview.has_value();
	}

	auto operator()() {
		if (is_terminated()) throw sequence_terminated_error();
		result res = std::move(*preview);
		_find_next();
		return res;
	}

private:
	void _find_next() {
		preview.reset();
		do {
			if (g.is_terminated()) return;
			preview.emplace(std::move(g()));
		} while (!p(polyfill::as_const_reference<result>(*preview)));
	}
};

template<typename Gen, typename Pred>
inline filteror<typename std::decay<Gen>::type, typename std::decay<Pred>::type> filter(Gen &&g, Pred &&p) {
	return {std::forward<Gen>(g), std::forward<Pred>(p)};
}

}

#endif
