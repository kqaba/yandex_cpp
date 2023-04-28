#pragma once 
#include "array_ptr.h" 
 
#include <cassert> 
#include <stdexcept> 
#include <initializer_list> 
#include <array> 
 
class ProxyReserve 
{ 
public: 
	ProxyReserve() = delete; 
	ProxyReserve(size_t size) 
		: _size(size) 
	{} 
 
	size_t GetSize() 
	{ 
		return _size; 
	} 
 
private: 
	size_t _size; 
}; 
 
ProxyReserve Reserve(size_t size) 
{ 
	return ProxyReserve(size); 
} 
 
template <typename Type> 
class SimpleVector 
{ 
public: 
	using Iterator = Type*; 
	using ConstIterator = const Type*; 
 
	SimpleVector() noexcept = default; 
 
	explicit SimpleVector(ProxyReserve proxy_size) 
	{ 
		const size_t new_size = proxy_size.GetSize(); 
		ArrayPtr<Type> newArray(new_size); 
		_data.swap(newArray); 
		_capacity = new_size; 
	} 
 
	explicit SimpleVector(size_t size) 
		: SimpleVector(size, Type()) { 
	} 
 
	SimpleVector(size_t size, const Type& value) 
		: _size(size), _capacity(size) 
	{ 
		ArrayPtr<Type> newArray(size); 
 
		std::fill(newArray.Get(), newArray.Get() + size, value); 
 
		_data.swap(newArray); 
	} 
 
	SimpleVector(std::initializer_list<Type> init) 
		: _size(init.size()), _capacity(_size) 
	{ 
		ArrayPtr<Type> newArray(_size); 
 
		std::copy(init.begin(), init.end(), newArray.Get()); 
 
		_data.swap(newArray); 
	} 
 
	SimpleVector(const SimpleVector& other) 
	{ 
		ArrayPtr<Type> newArray(other._capacity); 
 
		std::copy(other.begin(), other.end(), newArray.Get()); 
 
		_data.swap(newArray); 
		_size = other._size; 
		_capacity = other._capacity; 
	} 
 
	SimpleVector(SimpleVector&& other) 
	{ 
		_data.swap(other._data); 
		std::swap(_size, other._size); 
		std::swap(_capacity, other._capacity); 
	} 
 
SimpleVector& operator=(const SimpleVector& rhs) {  
		if (this != &rhs)  
        {  
				SimpleVector<Type> cop(rhs);  
				swap(cop);  
	    } 
		  
		return *this;  
	}  
 
SimpleVector& operator=(SimpleVector&& rhs)  
	{  
		if (this != &rhs)  
		{  
			SimpleVector<Type> tmp(std::move(rhs)); 
			this->swap(tmp);  
  
		}  
		return *this;  
	}  
 
	void PushBack(const Type& item) { 
		if (_size == _capacity) { 
			ReCapacity(std::max(_size * 2, static_cast<size_t>(1))); 
		} 
		*(end()) = item; 
		_size++; 
	} 
 
	void PushBack(Type&& item) { 
		if (_size == _capacity) { 
			ReCapacity(std::max(_size * 2, static_cast<size_t>(1))); 
		} 
		*(end()) = std::move(item); 
		_size++; 
	} 
 
 
	void PopBack() noexcept 
	{ 
		assert(!IsEmpty()); 
		--_size; 
 
	} 
 
	void Reserve(size_t new_capacity); 
	Iterator Insert(ConstIterator pos, const Type& value); 
	Iterator Insert(ConstIterator pos, Type&& value); 
	 
Iterator Erase(ConstIterator pos)  
	{  
		assert(pos >= begin() && pos < end());  
        auto distance = pos - cbegin(); 
        Iterator pos_ = _data.Get() + distance; 
        std::move(pos_ + 1, end(), pos_); 
        --_size; 
        return pos_; 
     
	}  
 
	void swap(SimpleVector& other) noexcept { 
		_data.swap(other._data); 
		std::swap(_size, other._size); 
		std::swap(_capacity, other._capacity); 
	} 
 
	size_t GetSize() const noexcept 
	{ 
		return _size; 
	} 
 
	size_t GetCapacity() const noexcept 
	{ 
		return _capacity; 
	} 
 
	bool IsEmpty() const noexcept 
	{ 
		return _size == 0; 
	} 
 
	Type& operator[](size_t index) noexcept 
	{ 
		return _data[index]; 
	} 
 
	const Type& operator[](size_t index) const noexcept 
	{ 
		return const_cast<const Type&>(_data[index]); 
	} 
 
	Type& At(size_t index) 
	{ 
		if (index >= _size) 
		{ 
			throw std::out_of_range("Out of range"); 
		} 
		return _data[index]; 
	} 
 
	const Type& At(size_t index) const 
	{ 
		if (index >= _size) 
		{ 
			throw std::out_of_range("Out of range"); 
		} 
		return  _data[index]; 
	} 
 
	void Clear() noexcept 
	{ 
		_size = 0; 
	} 
 
	void Resize(size_t new_size) 
	{ 
		if (new_size < _size) 
		{ 
			_size = new_size; 
		} 
		else if (new_size <= _capacity) 
		{ 
			auto iter = begin() + _size; 
			fill_non_copyble(iter, iter + (new_size - _size)); 
 
			_size = new_size; 
		} 
		else 
		{ 
			ArrayPtr<Type> newArray(new_size); 
 
			std::move(begin(), end(), newArray.Get()); 
			fill_non_copyble(newArray.Get() + _size, newArray.Get() + new_size); 
 
			_data.swap(newArray); 
			_capacity = _size = new_size; 
		} 
	} 
 
 
 
	Iterator begin() noexcept 
	{ 
		return _data.Get(); 
	} 
 
	Iterator end() noexcept 
	{ 
		return _data.Get() + _size; 
	} 
 
	ConstIterator begin() const noexcept 
	{ 
		return cbegin(); 
	} 
 
	ConstIterator end() const noexcept 
	{ 
		return cend(); 
	} 
 
	ConstIterator cbegin() const noexcept 
	{ 
		return const_cast<ConstIterator>(_data.Get()); 
	} 
 
	ConstIterator cend() const noexcept 
	{ 
		return const_cast<ConstIterator>(_data.Get() + _size); 
	} 
 
private: 
	ArrayPtr<Type> _data; 
	size_t _size = 0; 
	size_t _capacity = 0; 
	void ExpandCapacity(size_t new_capacity); 
 
	void fill_non_copyble(Iterator first, Iterator last) 
	{ 
         std::generate(first, last, []{return Type{};}); 
	} 
 
	void ReCapacity(size_t new_capacity) { 
		ArrayPtr<Type> new_cont_(new_capacity); 
		std::move(_data.Get(), _data.Get() + _size, new_cont_.Get()); 
		_data.swap(new_cont_); 
		_capacity = new_capacity; 
	} 
 
}; 
template <typename Type> 
void SimpleVector<Type>::Reserve(size_t new_capacity) { 
	if (new_capacity > _capacity) { 
		ExpandCapacity(new_capacity); 
	} 
} 
 
template <typename Type> 
Type* SimpleVector<Type>::Insert(const Type* pos, const Type& value) { 
	size_t shift = pos - begin(); 
	if (_size == _capacity) { 
		ExpandCapacity(_size == 0 ? 1 : _size * 2); 
	} 
	std::move_backward(begin() + shift, end(), end() + 1); 
	*(begin() + shift) = value; 
	++_size; 
	return begin() + shift; 
} 
 
template <typename Type> 
Type* SimpleVector<Type>::Insert(const Type* pos, Type&& value) { 
	size_t shift = pos - begin(); 
	if (_size == _capacity) { 
		ExpandCapacity(_size == 0 ? 1 : _size * 2); 
	} 
	std::move_backward(std::make_move_iterator(begin() + shift), std::make_move_iterator(end()), end() + 1); 
	*(begin() + shift) = std::move(value); 
	++_size; 
	return begin() + shift; 
} 
template <typename Type> 
void SimpleVector<Type>::ExpandCapacity(size_t new_capacity) { 
	ArrayPtr<Type> tmp(new_capacity); 
	for (size_t i = 0; i < _size; ++i) { 
		tmp[i] = std::move(_data[i]); 
	} 
	_data.swap(tmp); 
	_capacity = new_capacity; 
} 
template <typename Type> 
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{ 
	return lhs.GetSize() != rhs.GetSize() ? false : std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()); 
} 
 
template <typename Type> 
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{ 
	return !(lhs == rhs); 
} 
 
template <typename Type> 
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{ 
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()); 
} 
 
template <typename Type> 
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{ 
	return !(rhs < lhs); 
} 
 
template <typename Type> 
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{ 
	return rhs < lhs; 
} 
 
template <typename Type> 
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{ 
	return !(rhs < lhs); 
} 
