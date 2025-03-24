#pragma once
#include <assert.h>
#include <vector>

template <class T>
class Stack
{
public:
	//Skapar en tom stack
	Stack();
	//Returnerar antal element i stacken
	int GetSize() const;
	//Returnerar det översta elementet i stacken. Kraschar med en assert om
	//stacken är tom.
	const T& GetTop() const;
	//Returnerar det översta elementet i stacken. Kraschar med en assert om
	//stacken är tom.
	T& GetTop();
	//Lägger in ett nytt element överst på stacken
	void Push(const T& aValue);
	//Tar bort det översta elementet från stacken och returnerar det. Kraschar
	//med en assert om stacken är tom.
	T Pop();

	std::vector<T>& GetContainer() { return myContainer; }

	bool IsEmpty() const;

private:
	std::vector<T> myContainer;
};

template <class T>
Stack<T>::Stack()
{
}

template <class T>
int Stack<T>::GetSize() const
{
	return static_cast<int>(myContainer.size());
}

template <class T>
const T& Stack<T>::GetTop() const
{
	assert(myContainer.size() > 0);
	return myContainer[myContainer.size() - 1];
}

template <class T>
T& Stack<T>::GetTop()
{
	assert(myContainer.size() > 0 && "Stack is empty");
	return myContainer[myContainer.size() - 1];
}

template <class T>
void Stack<T>::Push(const T& aValue)
{
	myContainer.push_back(aValue);
}

template <class T>
T Stack<T>::Pop()
{
	assert(myContainer.size() > 0 && "Stack is empty");
	T temp = myContainer[myContainer.size() - 1];
	myContainer.pop_back();

	return temp;
}

template <class T>
bool Stack<T>::IsEmpty() const
{
	return myContainer.size();
}
