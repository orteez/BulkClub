#ifndef INITIALIZER_H_
#define INITIALIZER_H_

#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include "../member/Member.h"
#include "../member/Executive.h"
#include "../member/Regular.h"
#include "../util/Item.h"
#include "../util/Trip.h"

//this is happening, cry about it
#define MAX_ITEMS 256
#define TAX_RATE 0.0875
#define REBATE_PERCENTAGE 0.03

using namespace std;

Member* search_for_member(int, Member **, int);

Item* search_for_item(string, Item **, int);

bool Initialize_Everything(const int, const int, int &, Member **,
		Trip **, Item **, const int *, const char*);

#endif /* INITIALIZER_H_ */
