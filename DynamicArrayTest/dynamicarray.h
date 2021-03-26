#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H


template <class T>
class DynamicArray {
public:
	DynamicArray<T>();
	~DynamicArray<T>();

	//T& operator[](const unsigned short);

	void push(T*);
	T* pop();
	T* removeAtIndex(const unsigned short);
	T* insertAtIndex(T*, const unsigned short);

	T* getAtIndex(const unsigned short);
	T* getFirst() {return getAtIndex(0);};
	T* getLast() {return getAtIndex(firstOpenIndex - 1);}

	unsigned short getSize() const {return arraySize;}
	unsigned short getLength() const {return firstOpenIndex;}
private:
	T **createResizedArray(const unsigned short);
	void expand();
	bool shrink();

	const unsigned short ARRAY_BLOCK_SIZE = 16; //SET TO >36 TO OPTIMIZE KNOWN USE CASE (morse code hashtable)
	unsigned short arraySize;

	unsigned short firstOpenIndex;
	T **internalArray;
};


template <class T>
DynamicArray<T>::DynamicArray() {
	arraySize = ARRAY_BLOCK_SIZE;
	internalArray = new T*[arraySize]();
	firstOpenIndex = 0;
}


template <class T>
DynamicArray<T>::~DynamicArray() {
	for(int i = 0; i < firstOpenIndex; i += 1) {
		delete[] internalArray[i];
	}

	delete[] internalArray;
}


/*template <class T>
T& DynamicArray<T>::operator[](const unsigned short index) {
	if(index < firstOpenIndex) {
		return internalArray[index];
	}

	return nullptr;
}*/


template <class T>
T** DynamicArray<T>::createResizedArray(const unsigned short desiredSize) {
	T **resizedArray = new T*[desiredSize]();
	for(int i = 0; i < firstOpenIndex; i += 1) {
		resizedArray[i] = internalArray[i];
	}

	arraySize = desiredSize;
	return resizedArray; //implicit memory leak? one of these manually createed arrays is not being deleted.
}


template <class T>
void DynamicArray<T>::expand() {
	internalArray = createResizedArray(arraySize + ARRAY_BLOCK_SIZE);
	//delete[] internalArray; //WILL DELETING THE OLD ARRAY REMOVE DESIRED MEMORY? TODO: TEST!
}


template <class T>
bool DynamicArray<T>::shrink() {
	if(arraySize > ARRAY_BLOCK_SIZE) {
		internalArray = createResizedArray(arraySize - ARRAY_BLOCK_SIZE);
		//delete[] internalArray; //WILL DELETING THE OLD ARRAY REMOVE DESIRED MEMORY? TODO: TEST!
		return true;
	}

	return false;
}


template <class T>
void DynamicArray<T>::push(T* object) {
	if(firstOpenIndex == arraySize) {
		expand();
	}

	internalArray[firstOpenIndex++] = object;
}


template <class T>
T* DynamicArray<T>::pop() {
	return removeAtIndex(firstOpenIndex - 1);
}


template <class T>
T* DynamicArray<T>::removeAtIndex(const unsigned short index) {
	if(index >= firstOpenIndex) {
		return nullptr;
	}

	T* removedObject = internalArray[index];
	for(int i = index; i < firstOpenIndex; i += 1) {
		internalArray[i] = internalArray[i + 1];
	}

	firstOpenIndex -= 1;
	shrink();

	return removedObject;
}


template <class T>
T* DynamicArray<T>::getAtIndex(const unsigned short index) {
	if(index < firstOpenIndex) {
		return internalArray[index];
	}

	return nullptr;
}


template <class T>
T* DynamicArray<T>::insertAtIndex(T* object, const unsigned short index) {
	if (index < firstOpenIndex) {
		if(firstOpenIndex >= arraySize - 1) {//minus one to ensure there is room to expand by one
			expand();
		}

		for(int i = firstOpenIndex; i > index + 1; i -= 1) {
			internalArray[i] = internalArray[i - 1];
		}

		internalArray[index] = object;
		return object;
	}

	return nullptr;
}


#endif