#include <queue>
#include <unordered_set>
#include <vector>
#include <set>
#include <iostream>
#include "path.h"
#include "imdb-utils.h"
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

    string startActorName = argv[1];
    string targetActorName = argv[2];

    imdb db(kIMDBDataDirectory);
    if (!db.good()) {
        cerr << "Data directory not found!  Aborting..." << endl;
        return kDatabaseNotFound;
    }

    std::unordered_set<string> visitedActors;
    std::set<film> visitedFilms;
    std::queue<std::pair<string, path>> actorQueue;
    actorQueue.push({startActorName, path(startActorName)});
    visitedActors.insert(startActorName);
    size_t depth = 0;

    while (!actorQueue.empty() && depth < 7) {
        size_t size = actorQueue.size();

        for (size_t i = 0; i < size; i++) {
            std::pair<string, path> currentState = std::move(actorQueue.front());
            actorQueue.pop();
            const string& currentActor = currentState.first;
            const path& currentPath = currentState.second;

            std::vector<film> films;
            db.getCredits(currentActor, films);

            for (const auto& film : films) {
                if (visitedFilms.find(film) != visitedFilms.end()) {
                    continue;
                }
                visitedFilms.insert(film);
                std::vector<std::string> actors;
                db.getCast(film, actors);
                for (const auto& actor : actors) {
                    path newPath = currentPath;
                    newPath.addConnection(film, actor);
                    if (actor == targetActorName) {
                        cout << newPath << endl;
                        return 0;
                    }
                    if (visitedActors.find(actor) != visitedActors.end()) {
                        continue;
                    }
                    visitedActors.insert(actor);
                    actorQueue.push({actor, std::move(newPath)});
                }
            }
        }

        depth++;
    }

    cout << "No path between those two people could be found." << endl;

    return 1;
}
