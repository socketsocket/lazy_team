#ifndef WEBSERV_TRIPLET_HPP
#define WEBSERV_TRIPLET_HPP

template <class T1, class T2, class T3>
struct triplet {
	typedef T1	first_type;
	typedef T2	second_type;
	typedef T3	third_type;

	first_type	first;
	second_type	second;
	third_type	third;

	triplet(): first(), second(), third() {};
	template <class U, class V, class W>
	triplet(const triplet<U, V, W>& tr)
		: first(tr.first), second(tr.second), third(tr.third) {}
	triplet(const first_type& a, const second_type& b, const third_type, c)
		: first(a), second(b), third(c) {};

	triplet&	operator=(const triplet& tr) {
		first = tr.first;
		second = tr.second;
		third = tr.third;
		return *this;
	}
};

template <class T1, class T2, class T3>
bool	operator==(const triplet<T1, T2, T3>& lhs, const triplet<T1, T2, T3>& rhs) {
	return lhs.first == rhs.first && lhs.second == rhs.second && lhs.third == rhs.third;
}

template <class T1, class T2, class T3>
triplet<T1, T2, T3>	make_triplet(T1 x, T2 y, T3 z) {
	return triplet<T1, T2, T3>(x, y, z);
}

#endif