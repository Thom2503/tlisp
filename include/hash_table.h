#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "main.h"

#define INITIAL_CAP 128

struct Entry {
	const char *key;
	void *value;
};

struct Table {
	struct Entry *entries;
	size_t capacity;
	size_t length;
};

uint32_t hash(const char *str);
struct Table *table_create(void);
void table_destroy(struct Table *table);
void *table_get(struct Table *table, const char *key);
const char *table_set_entry(struct Entry *entries, size_t capacity, const char *key, void *value, size_t *plen);
bool table_expand(struct Table *table);
const char *table_set(struct Table *table, const char *key, void *value);

#endif
