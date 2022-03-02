#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

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
void LRUSim(uint32_t* addr, Cache* cache, int numHits);
void PLRUSim(uint32_t* addr, Cache* cache, int numHits);

int main(void) {

	uint32_t addr[] = { 0x158, 0x28c, 0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398, 0x2d4, 0x280, 0x24c, 0x2bc, 0x154, 0x280, 0x2f0, 0x2c0, 0x250,
		0x27c, 0x370, 0x394, 0x2dc, 0x284, 0x250, 0x2ac, 0x158, 0x28c, 0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398, 0x2d4, 0x280, 0x24c, 0x2bc,
		0x154, 0x280, 0x2f0, 0x2c0, 0x250, 0x27c, 0x370, 0x394, 0x2dc, 0x284, 0x250, 0x2ac, 0x158, 0x28c, 0x2fc, 0x200, 0x314, 0x344, 0x374,
		0x398, 0x2d4, 0x280, 0x24c, 0x2bc, 0x154, 0x280, 0x2f0, 0x2c0, 0x250, 0x27c, 0x370, 0x394, 0x2dc, 0x284, 0x250, 0x2ac, 0x158, 0x28c,
		0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398, 0x2d4, 0x280, 0x24c, 0x2bc, 0x154, 0x280, 0x2f0, 0x2c0, 0x250, 0x27c, 0x370, 0x394, 0x2dc,
		0x284, 0x250, 0x2ac };

	// Initialize cache for LRU
	Cache cache[4];
	initialize(cache);

	int numHits = 0;

	LRUSim(addr, cache, numHits);

	numHits = 0;

	// Reset cache for PLRU
	initialize(cache);

	PLRUSim(addr, cache, numHits);

	return 0;

}


// Simulates LRU cache
void LRUSim(uint32_t* addr, Cache* cache, int numHits) {

	int lru[4] = { 3, 2, 1, 0 };

	// Load addr into cache
	for (int i = 0; i < NUM_ADDR; i++) {

		// Parse values
		uint32_t cacheIndex = 0;
		uint32_t tagIndex = (addr[i] >> 4) & 0x3;
		uint32_t dataIndex = ((addr[i] >> 2) & 0xf);

		// I only want first 2 bits of dataIndex due data array stored inside tag
		uint32_t dataIndexUpdated = dataIndex & 0x3;
		uint32_t aTag = addr[i] >> 6;

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
			cacheIndex = hitIndex;

			// Update LRU
			LRU(lru, true, hitIndex);
			numHits++;
		}

		printf("    | Seq# %d   | Way# %d\n", i+1, (hitIndex == -1) ? cacheIndex : hitIndex);
		printf("| V |   c.tag = %d |   c.data   |    c.data.index = %d\n", aTag, dataIndex);
		printf("| %d |                    D0     |   c.tag.index  = %d\n", cache[cacheIndex].tag[tagIndex].valid, tagIndex);
		printf("|   |                    D1     |  History Stack = %d,%d,%d,%d\n", lru[0], lru[1], lru[2], lru[3]);
		printf("|   |                    D2     |         Status = %s\n", (hitIndex == -1) ? "Miss" : "Hit");
		printf("|   |                    D3     |\n");
		printf("\n\n");

	}

	printf("Number of hits: %d\n", numHits);
	printf("LRU Efficiency: %f", ((float)numHits / 96) * 100);
}

// Simulate pseudo-LRU
void PLRUSim(uint32_t* addr, Cache* cache, int numHits) {
	// Initialize pseudo-LRU
	int plru[3] = { 0 };

	// Load addr into cache using PLRU
	for (int i = 0; i < NUM_ADDR; i++) {

		// Parse values
		uint32_t cacheIndex = 0;
		uint32_t tagIndex = (addr[i] >> 4) & 0x3;
		uint32_t dataIndex = ((addr[i] >> 2) & 0xf);

		// I only want first 2 bits of dataIndex due data array stored inside tag
		uint32_t dataIndexUpdated = dataIndex & 0x3;
		uint32_t aTag = addr[i] >> 6;

		// Check for hit
		int hitIndex = hit(aTag, cache, tagIndex);

		// Update cache on miss and update PLRU
		if (hitIndex == -1) {
			cacheIndex = PLRU(plru, false, hitIndex);

			// Write to cache
			cache[cacheIndex].tag[tagIndex].value = aTag;
			cache[cacheIndex].tag[tagIndex].valid = 1;

			// Fill with fake data
			for (int k = 0; k < 4; k++) {
				cache[cacheIndex].tag[tagIndex].data[k] = k + 5;
			}

		}
		else {
			// Update PLRU
			cacheIndex = PLRU(plru, true, hitIndex);
			numHits++;
		}

		printf("    | Seq# %d   | Way# %d\n", i+1, (hitIndex == -1) ? cacheIndex : hitIndex);
		printf("| V |   c.tag   |   c.data   |    c.data.index = %d\n", dataIndex);
		printf("\n\n");
	}

	printf("Number of hits: %d\n", numHits);
	printf("PLRU Efficiency: %f", ((float)numHits / 96) * 100);
}
// Update PLRU for least recent used set
int PLRU(int* plru, bool hit, int hitIndex) {

	int currentIndex = -1;

	// Find current arrow positions
	if (plru[0] == 0) {
		currentIndex = plru[0] * 2 + plru[1];
	}
	else {
		currentIndex = plru[0] * 2 + plru[2];
	}

	int temp = plru[0];
	int temp1 = plru[1];
	int temp2 = plru[2];

	int resultSet = hitIndex;

	// Change PRLU based on hit index (0 = left pointer, 1 = right pointer)
	if (hit) {
		// Check if current arrow position is same as hit index
		if (currentIndex == hitIndex) {
			// Adjust pointers
			if (plru[0] == 0) {
				plru[1] = plru[1] == 0 ? 1 : 0;
				plru[0] = 1;
			}
			else {
				plru[2] = plru[2] == 0 ? 1 : 0;
				plru[0] = 0;

			}
		}
		else {
			// Only adjust bit of hit index
			switch (hitIndex) {
			case 0:
				plru[1] = 1;
				break;

			case 1:
				plru[1] = 0;
				break;

			case 2:
				plru[2] = 1;
				break;

			case 3:
				plru[2] = 0;
				break;
			}
		}
	}
	else {
		if (plru[0] == 0) {
			resultSet = plru[0] * 2 + plru[1];

			plru[1] = plru[1] == 0 ? 1 : 0;
			plru[0] = 1;
		}
		else {
			resultSet = plru[0] * 2 + plru[2];

			plru[2] = plru[2] == 0 ? 1 : 0;
			plru[0] = 0;
			
		}
	}

	return resultSet;

}

// Returns hit or miss if value is in cache
int hit(uint32_t aTag, Cache* c, uint32_t tagIndex) {
	// Loop through all caches
	for (int i = 0; i < NUM_WAY; i++) {
		if (c[i].tag[tagIndex].valid == 1) {
			if (c[i].tag[tagIndex].value == aTag) {
				return i;
			}
		}
	}
	return -1;
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