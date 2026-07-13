#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstdint>
#include <cassert>
#include "imdb.h"
using namespace std;

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
    : actorBase(acquireFileMap(directory + "/" + kActorFileName, actorInfo))
    , maxActorCount(*as<u32>(actorBase))
    , movieBase(acquireFileMap(directory + "/" + kMovieFileName, movieInfo))
    , maxMovieCount(*as<u32>(movieBase)) {}

bool imdb::good() const {
    return !((actorInfo.fd == -1) || (movieInfo.fd == -1));
}

imdb::~imdb() {
    releaseFileMap(actorInfo);
    releaseFileMap(movieInfo);
}


// 一条演员信息是一堆连续字节，固定顺序：
// 1. 演员名字：C语言字符串，带`\0`结尾；如果名字字节数是奇数，多补一个空字符，保证总字节偶数（内存对齐要求）
// 2. 2字节短整数：该演员一共演了多少部电影；如果前面总字节不是4的倍数，补两个空字符对齐4字节
// 3. 一串4字节偏移数字：每个数字指向movieFile里某部电影的位置，代表他参演的影片

actor imdb::getActorWithOffset(const u32 offset) const {
    if (offset >= actorInfo.fileSize) {
        throw std::out_of_range("offset out of range");
    }
    const void* base = actorBase;
    const u32 nameOffset = offset;
    string name(as<char>(base, nameOffset));
    const u32 movieCountOffset = alignUp(nameOffset + static_cast<u32>(name.size()) + 1, 2);
    const u16 movieCount = *as<u16>(as<u8>(base, movieCountOffset));
    const u32 movieCountValuesOffset = alignUp(movieCountOffset + 2, 4);
    std::vector<u32> movieOffsets;
    for (u16 j = 0; j < movieCount; j++) {
        movieOffsets.push_back(*as<u32>(as<u8>(base, movieCountValuesOffset), j));
    }

    return actor { std::move(name), movieCount, std::move(movieOffsets) };
}

actor imdb::getActor(const u32 index) const {
    if (index >= maxActorCount) {
        throw std::out_of_range("index out of range");
    }
    return getActorWithOffset(getItemOffset(actorBase, index));
}

// ### （2）单条电影记录二进制格式
// 1. 电影名字，带`\0`结尾
// 2. 1字节年份：只存「年份-1900」，比如1986就存86；如果前面字节总数奇数，补一个空字符对齐
// 3. 2字节短整数：这部电影有多少演员；按需补空字符4字节对齐
// 4. 一串4字节偏移数字：每个指向actorFile里一位演员，代表本片全部演员

film imdb::getFilmWithOffset(const u32 offset) const { 
    if (offset >= movieInfo.fileSize) {
        throw std::out_of_range("offset out of range");
    }
    const void* base = movieBase;
    const u32 titleOffset = offset;
    string title(as<char>(base, titleOffset));

    const u32 yearOffset = titleOffset + static_cast<u32>(title.size()) + 1;
    const int year = 1900 + (*as<u8>(base, yearOffset));

    const u32 actorCountOffset = alignUp(yearOffset + 1, 2);
    const u16 actorCount = *as<u16>(as<u8>(base, actorCountOffset));

    const u32 actorValueOffset = alignUp(actorCountOffset + 2, 4);
    std::vector<u32> actorOffsets;
    for (u16 j = 0; j < actorCount; j++) {
        u32 actorOffset = *as<u32>(as<u8>(base, actorValueOffset), j);
        actorOffsets.push_back(actorOffset);
    }

    return film { std::move(title), year, actorCount, std::move(actorOffsets) };
}

film imdb::getFilm(const u32 index) const {
    if (index >= maxMovieCount) {
        throw std::out_of_range("index out of range");
    }
    return getFilmWithOffset(getItemOffset(movieBase, index));
}

bool imdb::getCredits(const string& player, vector<film>& films) const { 
    auto begin = getItemOffsetPtr(actorBase, 0);
    auto end = getItemOffsetPtr(actorBase, maxActorCount);
    auto it = std::lower_bound(begin, end, player, [this](const u32& offset1, const string& player) {
        return getActorWithOffset(offset1).name < player;
    });
    if (it == end) {
        return false;
    }
    actor a = getActorWithOffset(*it);
    if (a.name != player) {
        return false;
    }
    for (const auto& offset : a.movies) {
        films.push_back(getFilmWithOffset(offset));
    }
    return true;
}

bool imdb::getCast(const film& movie, vector<string>& players) const {
    auto begin = getItemOffsetPtr(movieBase, 0);
    auto end = getItemOffsetPtr(movieBase, maxMovieCount);
    auto it = std::lower_bound(begin, end, movie, [this](const u32& offset1, const film& movie) {
        return getFilmWithOffset(offset1) < movie;
    });
    if (it == end) {
        return false;
    }
    film m = getFilmWithOffset(*it);
    if (m != movie) {
        return false;
    }
    for (const auto& offset : m.actors) {
        players.push_back(getActorWithOffset(offset).name);
    }

    return true;
}

u32 imdb::getItemOffset(const void* base, const u32 index) {
    return *getItemOffsetPtr(base, index);
}

const u32* imdb::getItemOffsetPtr(const void* base, const u32 index) {
    return as<u32>(base, index + 1);
}

const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info) {
    struct stat stats;
    stat(fileName.c_str(), &stats);
    info.fileSize = static_cast<size_t>(stats.st_size);
    info.fd = open(fileName.c_str(), O_RDONLY);
    return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info) {
    if (info.fileMap != NULL)
        munmap(const_cast<char*>(reinterpret_cast<const char*>(info.fileMap)), info.fileSize);
    if (info.fd != -1)
        close(info.fd);
}
