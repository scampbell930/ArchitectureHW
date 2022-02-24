#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#define NUM_LRU 4
#define NUM_WAY 4

typedef struct tag {
	uint32_t value;
	uint32_t valid;
} Tag;

typedef struct cache {
	uint32_t data[16];
	Tag tag[4];
} Cache;

// Prototypes
void displayCache(Cache* c);
void initialize(Cache* c);
void LRU(int* lru, bool hit, int index);

int main(void) {

	uint32_t addr[] = { 0x158, 0x28c, 0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398, 0x2d4, 0x280, 0x24c, 0x2bc, 0x154, 0x280, 0x2f0, 0x2c0, 0x250,
		0x27c, 0x370, 0x394, 0x2dc, 0x284, 0x250, 0x2ac, 0x158, 0x28c, 0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398, 0x2d4, 0x280, 0x2f0, 0x2c0, 0x250,
		0x27c, 0x370, 0x394, 0x2dc, 0x284, 0x250, 0x2ac, 0x158, 0x28c, 0x2fc, 0x200, 0x314, 0x344, 0x374, 0x398, 0x2d4, 0x280, 0x24c, 0x2bc, 0x154,
		0x280, 0x2f0, 0x2c0, 0x250, 0x27c, 0x370, 0x394, 0x2dc, 0x284, 0x250, 0x2ac, 0x158, 0x28c, 0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398, 0x2d4,
		0x280, 0x24c, 0x2bc, 0x154, 0x280, 0x2f0, 0x2c0, 0x250, 0x27c, 0x370, 0x394, 0x2dc, 0x284, 0x250, 0x2ac };

	int lru[4] = {3, 2, 1, 0};

	Cache cache[4];
	initialize(cache);

	return 0;

}

// Print cache contents
void displayCache(Cache* c) {
	for (int i = 0; i < NUM_WAY; i++) {
		printf("%d\n", i);
		int k = 0;
		for (int j = 0; j < (sizeof(c[i].tag) / sizeof(c[i].tag[j])); j++) {
			printf("Tag:\tValid: %d\tValue: %d\n", c[i].tag[j].valid, c[i].tag[j].value);

			for (int l = k; l < k + 4; l++) {
				printf("Data: %d\n", c[i].data[l]);
			}
			k += 4;
		}
		puts("\n\n\n");
	}
}

// Sets all data and tag values to 0, sets all valid bits to 0
void initialize(Cache* c) {
	for (int i = 0; i < NUM_WAY; i++) {

		// Clear data
		for (int j = 0; j < (sizeof(c[i].data) / sizeof(c[i].data[j])); j++) {
			c[i].data[j] = 0;
		}

		// Clear tag
		for (int j = 0; j < (sizeof(c[i].tag) / sizeof(c[i].tag[j])); j++) {
			c[i].tag[j].value = 0;
			c[i].tag[j].valid = 0;
		}

	}
}

// Executes an LRU update for a given lru, hit/miss, and line hit number
void LRU(int* lru, bool hit, int index) {
	// Check if hit or miss
	if (hit && index != 3) {
		// Move hit line to MRU position
		int mru = lru[index];

		for (int i = index; i >= 1; i--) {
			lru[i] = lru[i - 1];
		}

		lru[0] = mru;

	}
	else {
		// Move LRU to MRU position
		for (int i = NUM_LRU-1; i >= 1; i--) {
			int temp = lru[i - 1];
			lru[i - 1] = lru[i];
			lru[i] = temp;
		}

	}
}