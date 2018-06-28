#include "stdafx.h"
#include <string>
#include <cstddef>
#include "MemoryPool.h"
#define __MAX_BYTES 128

template<class _T>
class allocato {
public :
	typedef void _Not_user_specialized;
	typedef _T value_type;
	typedef value_type *pointer;
	typedef const value_type *const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef std::true_type propagate_on_container_move_assignment;
	typedef std::true_type is_always_equal;

	allocato() {}
	allocato(const allocato&) {}

	template <class U>
	allocato(const allocato<U>&) {}

	~allocato() {}

	pointer address(reference _Val) const _NOEXCEPT {
		return const_cost<reference>(address(static_cast<const_reference>(_Val)));
	}
	const_pointer address(const_reference _Val) const _NOEXCEPT {
		return &_Val;
	}
	pointer allocate(size_type _Count) {
		return static_cast<pointer>(getPool().allocate_pool(_Count * sizeof(_T)));
	}
	void deallocate(pointer _Ptr, size_type _Count) {
		getPool().deallocate_pool(_Ptr, _Count);
	}

	template<class _Uty>
	void destroy(_Uty *_Ptr) {
		delete (_Ptr);
	}
	template<class _Objty, class... _Types>
	void construct(_Objty *_Ptr, _Types&&... _Args) {
		::new ((void*)_Ptr) _Objty(::std::forward<_Types>(_Args)...);
	}
	template <typename _other> 
	struct rebind { typedef allocato<_other> other; };

private:
	static MemoryPool &getPool() {
		static MemoryPool pool;
		return pool;
	}

};
