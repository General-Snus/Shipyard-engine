#pragma once

//I too am quite done with comptr include and i will blatantly steal its interface
//Simple reference counted container
#include <type_traits>

template<typename T>
concept releaseable =  requires(T t)
{
	t.Release();//Make sure its a IUnknown instead or just fulfills its interface
};

template<releaseable T>
class RefenceCountedContainer
{
public:
	using InterfaceType = T;

public:
	RefenceCountedContainer(): ptr_(nullptr)
	{}

	explicit RefenceCountedContainer(decltype(__nullptr)): ptr_(nullptr)
	{}

	template<class U>
	explicit RefenceCountedContainer(U *other): ptr_(other)
	{
		InternalAddRef();
	}

	RefenceCountedContainer(const RefenceCountedContainer& other) noexcept: ptr_(other.ptr_)
	{
		InternalAddRef();
	}

	// copy constructor that allows to instantiate class when U* is convertible to T*
	template<class U>
	RefenceCountedContainer(const RefenceCountedContainer<U> &other,typename std::enable_if<std::is_convertible_v<U*,T*>,void *>::type * = 0) noexcept:
		ptr_(other.ptr_)
	{
		InternalAddRef();
	}

	RefenceCountedContainer(RefenceCountedContainer &&other) noexcept: ptr_(nullptr)
	{
		if(this != reinterpret_cast<RefenceCountedContainer*>(&reinterpret_cast<unsigned char&>(other)))
		{
			Swap(other);
		}
	}

	// Move constructor that allows instantiation of a class when U* is convertible to T*
	template<class U>
	RefenceCountedContainer(RefenceCountedContainer<U>&& other,typename std::enable_if<std::is_convertible_v<U*,T*>,void *>::type * = 0) noexcept:
		ptr_(other.ptr_)
	{
		other.ptr_ = nullptr;
	}

	~RefenceCountedContainer() noexcept
	{
		InternalRelease();
	}

	RefenceCountedContainer& operator=(decltype(__nullptr)) noexcept
	{
		InternalRelease();
		return *this;
	}

	RefenceCountedContainer& operator=(T *other) noexcept
	{
		if(ptr_ != other)
		{
			RefenceCountedContainer(other).Swap(*this);
		}
		return *this;
	}

	template <typename U>
	RefenceCountedContainer& operator=(U *other) noexcept
	{
		RefenceCountedContainer(other).Swap(*this);
		return *this;
	}

	RefenceCountedContainer& operator=(const RefenceCountedContainer &other) noexcept
	{
		if(ptr_ != other.ptr_)
		{
			RefenceCountedContainer(other).Swap(*this);
		}
		return *this;
	}

	template<class U>
	RefenceCountedContainer& operator=(const RefenceCountedContainer<U>& other) noexcept
	{
		RefenceCountedContainer(other).Swap(*this);
		return *this;
	}

	RefenceCountedContainer& operator=(RefenceCountedContainer &&other) noexcept
	{
		RefenceCountedContainer(static_cast<RefenceCountedContainer&&>(other)).Swap(*this);
		return *this;
	}

	template<class U>
	RefenceCountedContainer& operator=(RefenceCountedContainer<U>&& other) noexcept
	{
		RefenceCountedContainer(static_cast<RefenceCountedContainer<U>&&>(other)).Swap(*this);
		return *this;
	}


	void Swap(RefenceCountedContainer&& r) noexcept
	{
		T* tmp = ptr_;
		ptr_ = r.ptr_;
		r.ptr_ = tmp;
	}

	void Swap(RefenceCountedContainer& r) noexcept
	{
		T* tmp = ptr_;
		ptr_ = r.ptr_;
		r.ptr_ = tmp;
	}

	InterfaceType* Get() const
	{
		return ptr_;
	}

	operator bool() const {
		return Get() != nullptr;
	}

	operator T*() const
	{
		return Get();
	}

	InterfaceType* operator->() const
	{
		return ptr_;
	}

	T* const* GetAddressOf() const noexcept
	{
		return &ptr_;
	}

	T** GetAddressOf() noexcept
	{
		return &ptr_;
	}

	T** ReleaseAndGetAddressOf() noexcept
	{
		InternalRelease();
		return &ptr_;
	}

	T* Detach() noexcept
	{
		InterfaceType* ptr = ptr_;
		ptr_ = nullptr;
		return ptr;
	}

	void Attach(InterfaceType* other)
	{
		if(ptr_ != nullptr)
		{
			auto ref = ptr_->Release();
			(void)ref;
			assert(ref != 0 || ptr_ != other);
		}

		ptr_ = other;
	}

	unsigned long Reset()
	{
		return InternalRelease();
	}


	// query for U interface
	template<releaseable U>
	HRESULT As(RefenceCountedContainer<U>* p) const
	{
		return ptr_->QueryInterface(__uuidof(U),reinterpret_cast<void**>(p->ReleaseAndGetAddressOf()));
	}


protected:
	InterfaceType *ptr_;
	template<releaseable U> friend class RefenceCountedContainer;

	void InternalAddRef() const
	{
		if(ptr_ != nullptr)
		{
			ptr_->AddRef();
		}
	}

	unsigned long InternalRelease()
	{
		unsigned long ref = 0;
		T* temp = ptr_;

		if(temp != nullptr)
		{
			ptr_ = nullptr;
			ref = temp->Release();
		}

		return ref;
	}

};

template <releaseable T>
using Ref = RefenceCountedContainer<T>;