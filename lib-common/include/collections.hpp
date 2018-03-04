/*
 * List.h
 *
 *  Created on: 04.03.2018
 *      Author: fabia
 */

#ifndef LIB_COMMON_INCLUDE_COLLECTIONS_HPP_
#define LIB_COMMON_INCLUDE_COLLECTIONS_HPP_

template <typename T> struct Node {
	T data;
	Node<T>* next;
};

template <typename T> class List {
private:
	Node<T>* head;
	Node<T>* tail;
public:
	void add(T element) {
		tail->data = element;
		tail->next = new Node<T>();
		tail = tail->next;
	}

	void remove(T element) {
		Node<T>* prev = nullptr;
		Node<T>* current = head;
		do {
			if(current->data == element) {
				if(prev != nullptr) {
					prev->next = current->next;
					delete current;
				}
				else
				{
					head = current->next;
					delete current;
				}
			}

			prev = current;
			current = current->next;
		} while(current != nullptr);
	}

	T get(int index) {
		Node<T>* current = head;

		while(index-- && current != tail) {
			current = current->next;
		}

		return current->data;
	}

	List() {
		head = new Node<T>;
		tail = head;
	}

	~List() {
		do {
			Node<T>* ptr = head;
			head = head->next;

			delete ptr;
		} while (head != 0);
	}
};

#endif /* LIB_COMMON_INCLUDE_COLLECTIONS_HPP_ */
