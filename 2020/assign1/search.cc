#include <vector>
#include <iostream>
#include "imdb.h"

using namespace std;

static const int kWrongArgumentCount = 1;
static const int kDatabaseNotFound = 2;

// poohbear@myth10$ ./search "Danzel Muzingo" "Liseli Mutti"
// Danzel Muzingo was in "My Day in the Barrel" (1998) with Damian Brown.
// Damian Brown was in "Bad Chemistry" (1997) with Dick Welsbacher.
// Dick Welsbacher was in "The Attic" (1980) with Carrie Snodgress.
// Carrie Snodgress was in "Chill Factor" (1989) with Nathaniel Lees (I).
// Nathaniel Lees (I) was in "Rapa Nui" (1994) with Liseli Mutti.

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <actor> <actor>" << endl;
        return kWrongArgumentCount;
    }

    string actor1Name = argv[1];
    string actor2Name = argv[2];

    cout << actor1Name << " -> " << actor2Name << endl;

    imdb db(kIMDBDataDirectory);
    if (!db.good()) {
        cerr << "Data directory not found!  Aborting..." << endl;
        return kDatabaseNotFound;
    }

    return 0;
}
