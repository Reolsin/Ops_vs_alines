#pragma once
#include <exception>
#include <vector>

namespace My_vect {

	template <class T>
	class My_vectorIt;

	template<class T>
	class My_vector {
		friend class My_vectorIt<T>;
	private:
		static const size_t Q = 5;
		size_t cpt;
		size_t sz;
		T* a;
	public:
		My_vector() : cpt(Q), sz(0), a(new T[Q]) {}
		My_vector(const My_vector<T>&);
		My_vector(My_vector<T>&& v) noexcept : cpt(v.cpt), sz(v.sz), a(v.a) { v.a = nullptr; }
		My_vector(int, const T);
		My_vector(const std::vector<T>&);
		~My_vector() { delete[] a; }

		My_vector<T>& operator=(const My_vector<T>&);
		My_vector<T>& operator=(My_vector<T>&&);
		void resize(int);
		void push_back(const T&);
		void pop_back();
		T& operator[](const int i) { return a[i]; }
		const T& operator[](const int i) const { return a[i]; };
		T& at(const int);
		const T& at(const int) const;
		void clear() { sz = 0; cpt = Q; delete[] a; a = new T[Q]; }
		T& back() { return a[sz]; }
		const T& back() const { return a[sz]; }

		inline My_vectorIt<T> begin() { return My_vectorIt<T>(a); }
		inline My_vectorIt<T> end() { return My_vectorIt<T>(a + sz); }

		inline bool not_empty() const { return sz > 0; }
		inline bool is_full() const { return cpt == sz; }
		inline size_t capacity() const { return cpt; }
		inline size_t size() const { return sz; }
	};

	template <class T>
	class My_vectorIt {
	private:
		T* ptr;
	public:
		My_vectorIt() : ptr(nullptr) {}
		My_vectorIt(T* p) : ptr(p) {}
		bool operator!=(const My_vectorIt& it) const { return ptr != it.ptr; }
		bool operator==(const My_vectorIt& it) const { return ptr == it.ptr; }
		T& operator*() { return *ptr; }
		T* operator->() { return ptr; }
		My_vectorIt& operator++() { ++ptr; return ptr; }
		My_vectorIt operator++(int) { My_vectorIt<T> tmp(*this); ++ptr; return tmp; }
	};

	template<class T>
	T&& move(T&& a)
	{
		return a;
	}

	template<class T>
	void swap(T& a1, T& a2)
	{
		T tmp(move(a1)); a1 = move(a2); a2 = move(tmp);
	}

	template<class T>
	My_vector<T>::My_vector(const My_vector<T>& v) : sz(v.sz), cpt(v.cpt)
	{
		a = new T[cpt];
		for (int i = 0; i < sz; i++)
			a[i] = v.a[i];
	}

	template<class T>
	My_vector<T>::My_vector(int n_, const T raw) : sz(0)
	{
		if (n_ >= 0) {
			size_t n(n_);
			cpt = (1 + n / Q) * Q;
			a = new T[cpt];
			for (; sz < n; sz++)
				a[sz] = raw;
		}
		else throw std::exception("Number of raws cant be negative.");
	}

	template<class T>
	My_vector<T>::My_vector(const std::vector<T>& v) : sz(v.size()), cpt((1 + v.size() / Q)* Q)
	{
		a = new T[cpt];
		for (int i = 0; i < sz; i++)
			a[i] = v[i];
	}

	template<class T>
	My_vector<T>& My_vector<T>::operator=(const My_vector<T>& v)
	{
		if (this != &v) {
			sz = v.sz;
			cpt = v.cpt;
			delete[] a;
			a = new T[cpt];
			for (size_t i = 0; i < sz; i++)
				a[i] = v.a[i];
		}
		return *this;
	}

	template<class T>
	My_vector<T>& My_vector<T>::operator=(My_vector<T>&& v)
	{
		swap(sz, v.sz);
		swap(cpt, v.cpt);
		swap(a, v.a);
		return *this;
	}

	template<class T>
	void My_vector<T>::resize(int new_sz)
	{
		if (new_sz >= 0) {
			int new_cpt = (1 + new_sz / Q) * Q;
			if (new_cpt != (int)cpt) {
				cpt = new_cpt;
				T* tmp = a;
				a = new T[cpt];
				for (int i = 0; (i < new_sz) && (i < (int)sz); i++)
					a[i] = tmp[i];
				delete[] tmp;
			}
			sz = new_sz;
		}
	}

	template<class T>
	void My_vector<T>::push_back(const T& raw)
	{
		if (is_full()) {
			cpt += Q;
			T* tmp = a;
			a = new T[cpt];
			for (size_t i = 0; i < sz; i++)
				a[i] = tmp[i];
			delete[] tmp;
		}
		a[sz++] = raw;
	}

	template<class T>
	void My_vector<T>::pop_back()
	{
		if (not_empty()) {
			sz--;
			if ((sz * 4 <= cpt) && (cpt > Q)) {
				cpt = cpt / 2;
				T* tmp = a;
				a = new T[cpt];
				for (size_t i = 0; i < sz; i++)
					a[i] = tmp[i];
				delete[] tmp;
			}
		}
		else throw std::exception("Vector is empty!");
	}

	template<class T>
	T& My_vector<T>::at(const int i)
	{
		if ((i >= 0) && (i < (int)sz))
			return a[i];
		else throw std::exception("Out of range!");
	}

	template<class T>
	const T& My_vector<T>::at(const int i) const
	{
		if ((i >= 0) && (i < (int)sz))
			return a[i];
		else throw std::exception("Out of range!");
	}
}