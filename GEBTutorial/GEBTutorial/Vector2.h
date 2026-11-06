#pragma once
#include <iostream>

// Class for recreating STD's Vector class, using floats
// Vector2 is used for things like 2D position, direction, or anything to do with holding an x and y float value
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

	// Set a new vector2
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

	// Operators for addition, subtraction, and multiplication
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

// Class for recreating dynamic arrays that can grow and shrink
// Used for storing entities to batch update, or pass into the quadtree
// Based off of: https://www.geeksforgeeks.org/dsa/how-do-dynamic-arrays-work/
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
	// Other constructor to create a new copy of another dynamic array
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

	// Push a new value to the array
	void Push(const T& value)
	{
		// Increase the capacity of the array if it is full
		if (size == capacity)
		{
			Grow();
		}

		array[size] = value;
		++size;
	}
	// Pop / remove the element at the last stored index
	void Pop()
	{
		// Don't decrease size if the array is already empty
		if (size == 0)
		{
			return;
		}
		size--;

		// Shrink array if the size is small enough
		if (capacity > 4 && size <= capacity / 4)
		{
			Shrink();
		}
	}

	// Increase the maximum capacity of the array
	// Issue with the way it grows, since it will double the capacity it could allocate more memory than is needed
	void Grow()
	{
		// Create a temporary array
		int newCapacity = (capacity == 0) ? 4 : capacity * 2;
		T* temp = new T[newCapacity];

		// Put everything from the old array into the new temp array
		for (int i = 0; i < size; i++)
		{
			temp[i] = array[i];
		}

		// Set temp to the array, increase capacity value, and release old array to stop memory leaks
		delete[] array;
		array = temp;
		capacity = newCapacity;
	}
	// Decrease the maximum capacity of the array to release memory
	void Shrink()
	{
		// Do not decrease past 4 values
		if (capacity <= 4)
		{
			return;
		}

		// Halve the capacity
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
		
		// Create a temporary array, put everything from old array into it
		T* temp = new T[newCapacity];
		for (int i = 0; i < size; i++)
		{
			temp[i] = array[i];
		}

		// Set temp to the array, increase capacity value, and release old array to stop memory leaks
		delete[] array;
		array = temp;
		capacity = newCapacity;
	}

	// Search through the array for a specific value
	T Search(const T& value)
	{
		// Loop through every index in the array and compare values
		for (int i = 0; i < size; i++)
		{
			if (array[i] == value)
			{
				// Return the value found at the index
				return array[i];
			}
		}

		// If the value cannot be found, return nothing
		return T{};
	}

	// Insert a value at a specific index
	void Insert(const T& value, int index)
	{
		// Don't accept index values below 0 or greater than the array's size
		if (index < 0 || index > size)
		{
			return;
		}

		// If the array is already at max capacity, increase it
		if (size == capacity)
		{
			Grow();
		}

		// Move all values in indexes above requested index until it has hit the requested index
		for (int i = size; i > index; i--)
		{
			array[i] = array[i - 1];
		}
		array[index] = value;
		++size;
	}
	// Remove a value at a requested index
	void Remove(int index)
	{
		// Don't accept index values below 0 or greater than the array's size
		if (index < 0 || index >= size)
		{
			return;
		}

		// Move elements in array until hit the current size
		for (int i = index; i < size - 1; i++)
		{
			array[i] = array[i + 1];
		}

		// Decrease size by one, check if array needs to shrink
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

	bool IsEmpty()
	{
		return size == 0;
	}
	int GetSize() const
	{
		return size;
	}
	int GetCapacity() const
	{
		return capacity;
	}

	// Symbol operators
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

// Template struct for nodes for queues and stacks
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

// Queue class - FIFO - Enqueues items and dequeues them when needed
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

// Stack class - LIFO - Uses push to push back an item and pop to take off first item
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

// Linked list class for dynamic memory assertion and faster insertion and deletion
// Functionally works similar to a stack and queue, except a slight bit faster and able to remove data at specific points
// Based off of: https://www.geeksforgeeks.org/cpp/program-to-implement-singly-linked-list-in-c-using-class/
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

	// Add new data to the linked list
	void Add(T& _data)
	{
		// Create a new node, and set it's pointer to the location of the header, set header to new node
		Node<T>* newNode = new Node(_data);
		newNode->next = head;
		head = newNode;
	}
	// Remove data from the linked list
	void Remove(T& _data)
	{
		// Create new nodes for current and previous
		Node<T>* currentNode = head;
		Node<T>* previousNode = nullptr;

		// Keep looping while current node is not nullptr, and check if the current node's data is equal to the data to be removed
		while (currentNode != nullptr)
		{
			if (currentNode->data == _data)
			{
				// Safely remove the data from the list and ensure that all pointers work fine
				// Edge case for if the data is at the start, need to move head
				if (previousNode == nullptr)
				{
					head = currentNode->next;
				}
				else
				{
					previousNode->next = currentNode->next;
				}

				// Delete data
				delete currentNode;
				return;
			}

			// Move to the next pieces of data in the list
			previousNode = currentNode;
			currentNode = currentNode->next;
		}
	}

	// Check if the list contains a specific type of data
	bool Contains(T& _data)
	{
		Node<T>* currentNode = head;

		// Loop until all nodes have been visited
		while (currentNode != nullptr)
		{
			// Data found!
			if (currentNode->data == _data)
			{
				return true;
			}

			// Move to next data in the list
			currentNode = currentNode->next;
		}

		// Data is not found within the list
		return false;
	}

	bool IsEmpty()
	{
		return head == nullptr;
	}
	void Clear()
	{
		while (!IsEmpty())
		{
			Node<T>* nextNode = head;
			head = head->next;
			delete nextNode;
		}

		head = nullptr;
	}
private:
	Node<T>* head;
};