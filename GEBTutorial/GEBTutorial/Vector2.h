#pragma once
#include <iostream>

class Vector2
{
public:
	float x;
	float y;

	Vector2()
	{
		x = 0;
		y = 0;
	}
	Vector2(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	void Set(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	float Length()
	{
		return std::sqrt(x * x + y * y);
	}

	float dot(const Vector2& other)
	{
		return x * other.x + y * other.y;
	}

	Vector2 operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}
	Vector2 operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}
	Vector2 operator*(float scalar) const
	{
		return Vector2(x * scalar, y * scalar);
	}
};

template <typename T>
class DynamicArray
{
public:
	DynamicArray()
	{
		size = 0;
		capacity = 0;
		array = nullptr;
	}
	// Make an independent copy of another DynamicArray
	DynamicArray(const DynamicArray& other)
	{
		size = other.size;
		capacity = other.capacity;
		array = nullptr;
		
		if (capacity > 0)
		{
			array = new T[capacity];

			for (int i = 0; i < size; i++)
			{
				array[i] = other.array[i];
			}
		}
	}
	~DynamicArray()
	{
		delete[] array;
	}

	int GetSize() const
	{
		return size;
	}
	int GetCapacity() const
	{
		return capacity;
	}
	bool IsEmpty()
	{
		return size == 0;
	}

	void Push(const T& value)
	{
		if (size == capacity)
		{
			Grow();
		}

		array[size] = value;
		++size;
	}
	void Pop()
	{
		if (size == 0)
		{
			return;
		}
		size--;

		if (capacity > 4 && size <= capacity / 4)
		{
			Shrink();
		}
	}

	void Grow()
	{
		int newCapacity = (capacity == 0) ? 4 : capacity * 2;
		T* temp = new T[newCapacity];

		for (int i = 0; i < size; i++)
		{
			temp[i] = array[i];
		}

		delete[] array;
		array = temp;
		capacity = newCapacity;
	}
	void Shrink()
	{
		if (capacity <= 4)
		{
			return;
		}

		int newCapacity = capacity / 2;

		if (newCapacity < 4)
		{
			newCapacity = 4;
		}
		if (newCapacity < size)
		{
			newCapacity = size;
		}

		if (newCapacity == capacity)
		{
			return;
		}
		
		T* temp = new T[newCapacity];
		for (int i = 0; i < size; i++)
		{
			temp[i] = array[i];
		}

		delete[] array;
		array = temp;
		capacity = newCapacity;
	}

	T Search(const T& value)
	{
		for (int i = 0; i < size; i++)
		{
			if (array[i] == value)
			{
				return array[i];
			}
		}
		return T{};
	}

	void Insert(const T& value, int index)
	{
		if (index < 0 || index > size)
		{
			return;
		}

		if (size == capacity)
		{
			Grow();
		}

		for (int i = size; i > index; i--)
		{
			array[i] = array[i - 1];
		}
		array[index] = value;
		++size;
	}
	void Remove(int index)
	{
		if (index < 0 || index >= size)
		{
			return;
		}

		for (int i = index; i < size - 1; i++)
		{
			array[i] = array[i + 1];
		}

		--size;

		if (capacity > 4 && size <= capacity / 4)
		{
			Shrink();
		}
	}
	void Clear()
	{
		size = 0;
	}

	T& operator[](int index)
	{
		return array[index];
	}
	const T& operator[](int index) const
	{
		return array[index];
	}
	DynamicArray& operator=(const DynamicArray& other)
	{
		if (this == &other)
		{
			return *this;
		}

		T* newArray = nullptr;

		if (other.capacity > 0)
		{
			newArray = new T[other.capacity];

			for (int i = 0; i < other.size; i++)
			{
				newArray[i] = other.array[i];
			}
		}

		delete[] array;

		array = newArray;
		size = other.size;
		capacity = other.capacity;

		return *this;
	}
private:
	T* array;
	int size;
	int capacity;
};

template <typename T>
struct Node
{
	T data;
	Node<T>* next;

	Node(const T& _data)
	{
		data = _data;
		next = nullptr;
	}
};

template <typename T>
class Queue
{
public:
	Queue()
	{
		head = tail = nullptr;
	}
	~Queue()
	{
		Clear();
	}

	void Enqueue(const T& _data)
	{
		Node<T>* newNode = new Node(_data);

		if (tail == nullptr)
		{
			head = tail = newNode;
		}
		else
		{
			tail->next = newNode;
			tail = newNode;
		}
	}
	T Dequeue()
	{
		if (head == nullptr)
		{
			return T();
		}

		Node<T>* node = head;

		if (head != tail)
		{
			head = head->next;
		}
		else
		{
			head = tail = nullptr;
		}

		T _data = node->data;

		delete node;
		return _data;
	}

	T& Peek()
	{
		return head->data;
	}
	bool IsEmpty()
	{
		return head == nullptr;
	}

	void Clear()
	{
		while (!IsEmpty())
		{
			Dequeue();
		}
	}
private:
	Node<T>* head;
	Node<T>* tail;
};

template <typename T>
class Stack
{
public:
	Stack()
	{
		head = nullptr;
	}
	~Stack()
	{
		Clear();
	}

	void Push(T& _data)
	{
		Node<T>* newNode = new Node(_data);
		newNode->next = head;
		head = newNode;
	}
	T Pop()
	{
		if (head == nullptr)
		{
			return T();
		}

		Node<T>* newNode = head;
		head = head->next;
		T data = newNode->data;

		delete newNode;
		return data;
	}

	T& Peek()
	{
		return head->data;
	}
	bool IsEmpty()
	{
		return head == nullptr;
	}
	void Clear()
	{
		while (!IsEmpty())
		{
			Pop();
		}
	}
private:
	Node<T>* head;
};

template <typename T>
class LinkedList
{
public:
	LinkedList()
	{
		head = nullptr;
	}
	~LinkedList()
	{
		Clear();
	}

	void Add(T& _data)
	{
		Node<T>* newNode = new Node(_data);
		newNode->next = head;
		head = newNode;
	}
	void Remove(T& _data)
	{
		Node<T>* currentNode = head;
		Node<T>* previousNode = nullptr;

		while (currentNode != nullptr)
		{
			if (currentNode->data == _data)
			{
				if (previousNode == nullptr)
				{
					head = currentNode->next;
				}
				else
				{
					previousNode->next = currentNode->next;
				}

				delete currentNode;
				return;
			}

			previousNode = currentNode;
			currentNode = currentNode->next;
		}
	}

	bool Contains(T& _data)
	{
		Node<T>* currentNode = head;

		while (currentNode != nullptr)
		{
			if (currentNode->data == _data)
			{
				return true;
			}

			currentNode = currentNode->next;
		}

		return false;
	}

	bool IsEmpty()
	{
		return head == nullptr;
	}
	void Clear()
	{
		Node<T>* currentNode = head;

		while (!IsEmpty())
		{
			Node<T>* nextNode = currentNode->next;
			head = head->next;
			delete currentNode;
		}

		head = nullptr;
	}
private:
	Node<T>* head;
};