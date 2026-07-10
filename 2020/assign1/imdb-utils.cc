#include "imdb-utils.h"


std::ostream& operator<<(std::ostream& os, const actor& actor) {
    os << std::showbase << std::hex
       << "{name: " << actor.name 
       << ", movieCount: " << actor.movieCount 
       << ", movies: [";
    bool first = true;
    for (const auto& movie : actor.movies) {
        if (!first) {
            os << ", ";
        } else {
            first = false;
        }
        os << movie;
    }
    os << "]}" << std::noshowbase << std::dec;
    return os;
}


std::ostream& operator<<(std::ostream& os, const film& film) {
    os << "{title: " << film.title
       << ", year: " << film.year
       << "}";
    return os;
}