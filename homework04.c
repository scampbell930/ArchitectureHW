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
void initialize(Cache* c);
void LRU(int* lru, bool hit, int index);
void LRUSim(uint32_t* addr);
void PLRUSim(uint32_t* addr);

int main(void) {

	uint32_t addr[] = { 0x158, 0x28c, 0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398, 0x2d4, 0x280,
						0x24c, 0x2bc, 0x154, 0x280, 0x2f0, 0x2C0, 0x250, 0x27C, 0x370, 0x394,
						0x2dC, 0x284, 0x250, 0x2aC };

	LRUSim(addr);
	PLRUSim(addr);

	return 0;

}


// Simulates LRU cache
void LRUSim(uint32_t* addr) {
	printf("LRU SIMULATION\n\n");

	// Initialize cache for LRU
	Cache cache[4];
	initialize(cache);

	int numHits = 0;
	int lru[4] = { 3, 2, 1, 0 };

	// Loop through addresses
	for (int l = 0; l < 4; l++) {
		for (int i = 0; i < 24; i++) {
			// Parse data
			int tagIndex = (addr[i] >> 4) & 0x3;
			int dataIndex = (addr[i] >> 2) & 0xf;
			int aTag = addr[i] >> 6;

			int hit = -1;

			// Check if hit or miss
			for (int j = 0; j < 4; j++) {
				if (cache[j].tag[tagIndex].valid == 1 && cache[j].tag[tagIndex].value == aTag) {
					hit = j;
				}
			}

			int cacheHit = 0;

			// Fill cache and update LRU
			if (hit != -1) {
				numHits++;

				LRU(lru, true, hit);
				cacheHit = lru[0];
			}
			else {
				LRU(lru, false, hit);
				cacheHit = lru[0];

				// Write to cache
				cache[cacheHit].tag[tagIndex].value = aTag;
				cache[cacheHit].tag[tagIndex].valid = 1;
			}

			printf("    | Seq# %d, 0x%x| Way# %d\n", (i + 1) + (l * 24), addr[i], cacheHit);
			printf("| V |   c.tag = %d |   c.data   |    c.data.index = %d\n", aTag, dataIndex);
			printf("| %d |                    D0     |   c.tag.index  = %d\n", cache[cacheHit].tag[tagIndex].valid, tagIndex);
			printf("|   |                    D1     |  History Stack = %d,%d,%d,%d\n", lru[0], lru[1], lru[2], lru[3]);
			printf("|   |                    D2     |         Status = %s\n", (hit == -1) ? "Miss" : "Hit");
			printf("|   |                    D3     |\n");
			printf("\n\n");
		}

	}

	printf("Number of LRU hits: %d\n", numHits);
	printf("LRU Efficiency: %f\n\n", ((float)numHits / 96) * 100);
	puts("");
}

// Simulate pseudo-LRU
void PLRUSim(uint32_t* addr) {
	printf("\n\n\n");
	printf("PSEUDO-LRU SIMULATION:\n\n");

	// Initialize cache for PLRU
	Cache cache[4];
	initialize(cache);

	int numHits = 0;

	// Initialize pseudo-LRU
	int plru[3] = { 0 };

	// Load addr into cache using PLRU
	for (int l = 0; l < 4; l++) {
		for (int i = 0; i < 24; i++) {

			// Parse values
			uint32_t cacheIndex = 0;
			uint32_t tagIndex = (addr[i] >> 4) & 0x3;
			uint32_t dataIndex = ((addr[i] >> 2) & 0xf);
			uint32_t aTag = addr[i] >> 6;
			int hitIndex = -1;

			// Check if hit or miss
			for (int j = 0; j < 4; j++) {
				if (cache[j].tag[tagIndex].valid == 1 && cache[j].tag[tagIndex].value == aTag) {
					hitIndex = j;
				}
			}

			// Update cache on miss and update PLRU
			if (hitIndex == -1) {
				cacheIndex = PLRU(plru, false, hitIndex);

				// Write to cache
				cache[cacheIndex].tag[tagIndex].value = aTag;
				cache[cacheIndex].tag[tagIndex].valid = 1;

			}
			else {
				// Update PLRU
				cacheIndex = PLRU(plru, true, hitIndex);
				numHits++;
			}

			printf("    | Seq# %d, 0x%x| Way# %d\n", (i + 1) + (l * 24), addr[i], cacheIndex);
			printf("| V |   c.tag = %d |   c.data   |    c.data.index = %d\n", aTag, dataIndex);
			printf("| %d |                    D0     |   c.tag.index  = %d\n", cache[cacheIndex].tag[tagIndex].valid, tagIndex);
			printf("|   |                    D1     |  History Stack = b%d%d%d\n", plru[2], plru[0], plru[1]);		// Adjusting printing of PRLU arrows to fit example from class (my arrangement works the same way, but arrows are in different order)
			printf("|   |                    D2     |         Status = %s\n", (hitIndex == -1) ? "Miss" : "Hit");
			printf("|   |                    D3     |\n");
			printf("\n\n");
		}
	}

	printf("Number of PLRU hits: %d\n", numHits);
	printf("PLRU Efficiency: %f", ((float)numHits / 96) * 100);
	puts("");
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

// Sets all data and tag values to 0, sets all valid bits to 0
void initialize(Cache* c) {
	for (int i = 0; i < NUM_WAY; i++) {

		// Clear tag
		for (int j = 0; j < (sizeof(c[i].tag) / sizeof(c[i].tag[j])); j++) {
			c[i].tag[j].value = 0;
			c[i].tag[j].valid = 0;
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
	if (index != -1) {
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