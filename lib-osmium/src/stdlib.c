#include "stdlib.h"
#include "memory.h"
#include "stdmem.h"
#include "stddef.h"

struct memory_node* first_unused = 0;
struct memory_node* first_used = 0;
struct memory_node* first_free = 0;

static void remove_from_list(struct memory_node** root, struct memory_node* element) {
	struct memory_node* last = 0;
	struct memory_node* cur = *root;

	while (cur != NULL) {
		if (cur == element) {
			if (last == NULL) {
				*root = cur->next;
				return;
			} else {
				last->next = cur->next;
				return;
			}
		}
		last = cur;
		cur = cur->next;
	}
}

static void append_to_list(struct memory_node** root, struct memory_node* element) {
	element->next = *root;
	*root = element;
}

static struct memory_node* pop_from_list(struct memory_node** root) {
	if (*root == NULL) return NULL;
	struct memory_node* pop = *root;
	remove_from_list(root, pop);
	return pop;
}

static void allocate_unused_nodes() {
	struct memory_node* new_nodes = pcalloc(1);
	memset(new_nodes, 0, PAGESIZE);

	for (uint32_t i = 1; i < (PAGESIZE / sizeof(struct memory_node)); i++) {
		append_to_list(&first_unused, &(new_nodes[i]));
	}
}

static struct memory_node* pop_unused_node() {
	struct memory_node* ret = pop_from_list(&first_unused);

	while (ret == NULL) {
		allocate_unused_nodes();
		ret = pop_from_list(&first_unused);
	}

	return ret;
}

static void merge_into_frees(struct memory_node* tf) {
	remove_from_list(&first_used, tf);

	struct memory_node* cur;

	editedList: cur = first_free;

	while (cur != NULL) {
		if (cur->address + cur->size == tf->address) {
			tf->address = cur->address;
			remove_from_list(&first_free, cur);
			goto editedList;
		}

		if (cur->address == tf->address + tf->size) {
			tf->size += cur->size;
			remove_from_list(&first_free, cur);
			goto editedList;
		}

		cur = cur->next;
	}

	append_to_list(&first_free, tf);
}

uint32_t malloced = 0;

void* malloc(size_t size) {
	if (size == 0)
		return NULL;

	malloced += size;

	struct memory_node* cur = first_free;

	while (cur != NULL) {
		if (cur->size >= size) {
			break;
		}
		cur = cur->next;
	}

	if (cur == NULL) {
		uint32_t pgs = size / PAGESIZE;

		if ((size % PAGESIZE) != 0)
			pgs++;

		void* addr = pcalloc(pgs);

		struct memory_node* fill = pop_unused_node();

		fill->address = (uint32_t) addr;
		fill->size = (uint32_t) size;

		if (pgs * PAGESIZE > size) {
			struct memory_node* free = pop_unused_node();

			free->address = fill->address + fill->size;
			free->size = pgs * PAGESIZE - size;

			append_to_list(&first_free, free);
		}

		append_to_list(&first_used, fill);

		return (void*) fill->address;
	} else {
		uint32_t freesize = cur->size - size;

		cur->size = size;

		remove_from_list(&first_free, cur);
		append_to_list(&first_used, cur);

		if (freesize > 0) {
			struct memory_node* free = pop_unused_node();

			free->address = cur->address + cur->size;
			free->size = freesize;

			append_to_list(&first_free, free);
		}

		return (void*) cur->address;
	}
}

void* calloc(size_t num, size_t size) {
	size_t gsize = num * size;
	void* p = malloc(gsize);

	if (p != NULL) {
		memset(p, 0, gsize);
	}

	return p;
}

void* realloc(void* ptr, size_t size) {
	struct memory_node* cur = first_used;

	while (cur != NULL) {
		if (cur->address == (uint32_t) ptr) {
			if (size == 0) {
				free(ptr);
				return NULL;
			} else {
				void* new = malloc(size);
				memcpy(new, (void*) cur->address, cur->size);
				free((void*) cur->address);

				return new;
			}
		}
		cur = cur->next;
	}
	return NULL;
}

void free(void* ptr) {
	struct memory_node* cur = first_used;

	while (cur != NULL) {
		if (cur->address == (uint32_t) ptr) {
			memset(ptr, 0, cur->size);
			malloced -= cur->size;

			merge_into_frees(cur);
			break;
		}
		cur = cur->next;
	}
}
