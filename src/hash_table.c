#include "../include/hash_table.h"

inline uint32_t hash(const char *str) {
	unsigned int byte, crc, mask;
	int i = 0, j;
	crc = 0xFFFFFFFF;
	while (str[i] != 0) {
		byte = str[i];
		crc ^= byte;
		for (j = 7; j >= 0; j--) {
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
		i++;
	}
	return ~crc;
}

struct Table *table_create(void) {
	struct Table *table = (struct Table *)malloc(sizeof(struct Table));
	if (table == NULL)
		return NULL;
	table->length = 0;
	table->capacity = INITIAL_CAP;

	table->entries = (struct Entry *)calloc(table->capacity, sizeof(struct Entry));
	if (table->entries == NULL) {
		free(table);
		return NULL;
	}
	return table;
}

void table_destroy(struct Table *table) {
	for (size_t i = 0; i < table->capacity; i++)
		free((void *)table->entries[i].key);

	free(table->entries);
	free(table);
}

void *table_get(struct Table *table, const char *key) {
	uint32_t h = hash(key);
	size_t index = (size_t)(h & (uint64_t)(table->capacity - 1));

	while (table->entries[index].key != NULL) {
		if (strcmp(key, table->entries[index].key) == 0)
			return table->entries[index].value;
		index++;
		if (index >= table->capacity)
			index = 0;
	}
	return NULL;
}

const char *table_set_entry(struct Entry *entries, size_t capacity, const char *key, void *value, size_t *plen) {
	uint32_t h = hash(key);
	size_t index = (size_t)(h & (uint64_t)(capacity - 1));

	while (entries[index].key != NULL) {
		if (strcmp(key, entries[index].key) == 0) {
			entries[index].value = value;
			return entries[index].key;
		}
		index++;
		if (index >= capacity)
			index = 0;
	}

	if (plen != NULL) {
		key = strdup(key);
		if (key == NULL)
			return NULL;
		(*plen)++;
	}
	entries[index].key = (char*)key;
	entries[index].value = value;
	return key;
}

bool table_expand(struct Table *table) {
	size_t new_capacity = table->capacity * 2;
	if (new_capacity < table->capacity)
		return false;
	struct Entry *new_entries = (struct Entry *)calloc(new_capacity, sizeof(struct Entry));
	if (new_entries == NULL)
		return false;

	for (size_t i = 0; i < table->capacity; i++) {
		struct Entry entry = table->entries[i];
		if (entry.key != NULL)
			table_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
	}

	free(table->entries);
	table->entries = new_entries;
	table->capacity = new_capacity;
	return true;
}

const char *table_set(struct Table *table, const char *key, void *value) {
	assert(value != NULL);
	if (value == NULL)
		return NULL;

	if (table->length >= table->capacity / 2)
		if (!table_expand(table))
			return NULL;
	return table_set_entry(table->entries, table->capacity, key, value, &table->length);
}
