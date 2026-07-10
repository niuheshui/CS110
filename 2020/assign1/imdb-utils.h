#pragma once
#include <vector>
#include <string>
#include <iostream>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

template<typename T>
const T* as(const void* p, u32 offset = 0) {
    return reinterpret_cast<const T*>(p) + offset;
}

inline u32 alignUp(u32 value, u32 alignment) {
    return (value + alignment - 1) / alignment * alignment;
}

const std::string kIMDBDataDirectory("/Users/justin/proj/CS110/2020/assign1/slink/");

/**
 * Convenience struct: film
 * ------------------------
 * Bundles the name of a film and the year it was made
 * into a single struct.  It is a true struct in that the 
 * client is free to access both fields and change them at will
 * without issue.  operator== and operator< are implemented
 * so that films can be stored in STL containers requiring 
 * such methods.
 */
struct film {
  
  std::string title;
  int year;
  u32 actorCount;
  std::vector<u32> actors;
  
  /** 
   * Methods: operator==
   *          operator<
   * -------------------
   * Compares the two films for equality, where films are considered to be equal
   * if and only if they share the same name and appeared in the same year.  
   * film is considered to be less than another film if its title is 
   * lexicographically less than the second title, or if their titles are the same 
   * but the first's year is precedes the second's.
   *
   * @param rhs the film to which the receiving film is being compared.
   * @return boolean value which is true if and only if the required constraints
   *         between receiving object and argument are met.
   */

  bool operator==(const film& rhs) const { 
    return this->title == rhs.title && (this->year == rhs.year); 
  }
  
  bool operator<(const film& rhs) const { 
    return 
      (this->title < rhs.title) || 
      (this->title == rhs.title && this->year < rhs.year);
  }
};

struct actor {
  std::string name;
  u16 movieCount;
  std::vector<u32> movies;

  bool operator==(const actor& rhs) const { 
    return this->name == rhs.name;
  }
  
  bool operator<(const actor& rhs) const { 
    return this->name < rhs.name;
  }
};

std::ostream& operator<<(std::ostream& os, const actor& actor);
std::ostream& operator<<(std::ostream& os, const film& film);