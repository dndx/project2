#include <utility>
#include <cstdio>
#include "utils.h"

using namespace std;

pair<int, int> extract_pair(const char *s)
{
    pair<int, int> result;
    int ret = sscanf(s, "%d..%d", &result.first, &result.second);
    if (ret != 2)
        FATAL("invalid pair format %s", s);

    return result;
}

