#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#define NUM_LRU 4
#define NUM_WAY 4
#define NUM_ADDR 96

typedef struct tag {
	uint32_t value;
	uint32_t valid;
	uint32_t data[4];
} Tag;

typedef struct cache {
	Tag tag[4];
} Cache;

// Prototypes
void displayCache(Cache* c);
void initialize(Cache* c);
void LRU(int* lru, bool hit, int index);
int hit(uint32_t aTag, Cache* c, uint32_t tagIndex);

int main(void) {

	uint32_t addr[] = { 0x158, 0x28c, 0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398, 0x2d4, 0x280, 0x24c, 0x2bc, 0x154, 0x280, 0x2f0, 0x2c0, 0x250,
		0x27c, 0x370, 0x394, 0x2dc, 0x284, 0x250, 0x2ac, 0x158, 0x28c, 0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398, 0x2d4, 0x280, 0x24c, 0x2bc,
		0x154, 0x280, 0x2f0, 0x2c0, 0x250, 0x27c, 0x370, 0x394, 0x2dc, 0x284, 0x250, 0x2ac, 0x158, 0x28c, 0x2fc, 0x200, 0x314, 0x344, 0x374,
		0x398, 0x2d4, 0x280, 0x24c, 0x2bc, 0x154, 0x280, 0x2f0, 0x2c0, 0x250, 0x27c, 0x370, 0x394, 0x2dc, 0x284, 0x250, 0x2ac, 0x158, 0x28c,
		0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398, 0x2d4, 0x280, 0x24c, 0x2bc, 0x154, 0x280, 0x2f0, 0x2c0, 0x250, 0x27c, 0x370, 0x394, 0x2dc,
		0x284, 0x250, 0x2ac };

	int lru[4] = { 3, 2, 1, 0 };

	Cache cache[4];
	initialize(cache);

	int numHits = 0;

	// Load addr into cache
	for (int i = 0; i < NUM_ADDR; i++) {

		// Parse values
		uint32_t cacheIndex = 0;
		uint32_t tagIndex = (addr[i] >> 4) & 0x3;
		uint32_t dataIndex = ((addr[i] >> 2) & 0xf);

		// I only want first 2 bits of dataIndex due data array stored inside tag
		uint32_t dataIndexUpdated = dataIndex & 0x3;
		uint32_t aTag = addr[i] >> 6;

		// Print parsed values
		printf("0x%x\t%d\t%d\t%d\n", addr[i], tagIndex, dataIndex, aTag);

		// Check for hit
		int hitIndex = hit(aTag, cache, tagIndex);

		// Update cache on miss and update LRU
		if (hitIndex == -1) {
			cacheIndex = lru[3];

			// Write to cache
			cache[cacheIndex].tag[tagIndex].value = aTag;
			cache[cacheIndex].tag[tagIndex].valid = 1;
			
			// Fill with fake data
			for (int k = 0; k < 4; k++) {
				cache[cacheIndex].tag[tagIndex].data[k] = k + 5;
			}

			// Update LRU
			LRU(lru, false, hitIndex);
		}
		else {
			// Update LRU
			LRU(lru, true, hitIndex);
			numHits++;
		}
	}

	printf("Number of hits: %d\n", numHits);
	printf("LRU Efficiency: %f", ((float)numHits / 96) * 100);

	return 0;

}

// Returns hit or miss if value is in cache
int hit(uint32_t aTag, Cache* c, uint32_t tagIndex) {
	// Loop through all caches
	for (int i = 0; i < NUM_WAY; i++) {
		if (((aTag & c[i].tag[tagIndex].value) == aTag) && (c[i].tag[tagIndex].valid == 1)) {
			return i;
		}
	}
	return -1;
}

// Print cache contents
void displayCache(Cache* c) {
	for (int i = 0; i < NUM_WAY; i++) {
		printf("\n\nWay %d\n", i);
		for (int j = 0; j < (sizeof(c[i].tag) / sizeof(c[i].tag[j])); j++) {
			printf("Tag:\tValid: %d\tValue: %d\n", c[i].tag[j].valid, c[i].tag[j].value);

			for (int l = 0; l < 4; l++) {
				printf("Data: %d\n", c[i].tag[j].data[l]);
			}
		}
		puts("\n\n\n");
	}
}

// Sets all data and tag values to 0, sets all valid bits to 0
void initialize(Cache* c) {
	for (int i = 0; i < NUM_WAY; i++) {

		// Clear tag
		for (int j = 0; j < (sizeof(c[i].tag) / sizeof(c[i].tag[j])); j++) {
			c[i].tag[j].value = 0;
			c[i].tag[j].valid = 0;

			for (int k = 0; k < 4; k++) {
				c[i].tag[j].data[k] = 0;
			}
		}

	}
}

// Executes an LRU update for a given lru, hit/miss, and line hit number
void LRU(int* lru, bool hit, int index) {
	// Change set index to LRU index
	for (int i = 0; i < 4; i++) {
		if (lru[i] == index) {
			index = i;
			break;
		}
	}

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
		for (int i = NUM_LRU - 1; i >= 1; i--) {
			int temp = lru[i - 1];
			lru[i - 1] = lru[i];
			lru[i] = temp;
		}

	}
}