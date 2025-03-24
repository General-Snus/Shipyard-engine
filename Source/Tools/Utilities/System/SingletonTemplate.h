#pragma once
#include "Executable/Executable/Export.h"
#include "Tools/Utilities/System/ServiceLocator.h"
#include "Tools/Utilities/TemplateHelpers.h"
#include <mutex>

// We inherite from this to store it in the service locator
// we can also delete the copy here
class SHIPYARD_API Singleton
{
	friend class ServiceLocator;

  public:
	Singleton() = default;
	~Singleton() = default;
};

//Only works with POD
template<class T>// requires NotSmartPointerType<T>
class DoubleBuffered {
public:	
	const T& Read() const;
	T& Write();
	void Write(T& t);

	void ChangeBuffer() {
		std::lock_guard guard(bufferChangeLock);
		alphaIsWrite = !alphaIsWrite;
		betaBuffer = alphaBuffer;
	}

	T& operator->() { return Write(); }
	const T& operator->() const { return Read(); }
private: 
	std::mutex bufferChangeLock;
	std::atomic_bool alphaIsWrite;
	T alphaBuffer;
	T betaBuffer;
};

template<class T>
inline const T& DoubleBuffered<T>::Read() const {
	return alphaIsWrite ? betaBuffer: alphaBuffer;
}

template<class T>
inline T& DoubleBuffered<T>::Write() {
	return alphaIsWrite ? alphaBuffer : betaBuffer	 ;
}

template<class T>
inline void DoubleBuffered<T>::Write(T& t) {  alphaIsWrite ? alphaBuffer : betaBuffer = t; }
 