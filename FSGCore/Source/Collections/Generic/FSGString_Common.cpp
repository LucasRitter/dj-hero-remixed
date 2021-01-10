//
// Created by lucas on 2020-12-30.
//

#include <Collections/FSGString.hpp>
#include <Core/FSGAssert.hpp>
#include <cstring>
#include <cstdlib>

void CString::StringCopy(char *pszTarget, unsigned int uTargetCapacity, const char *pszSource) {
    FSG_ASSERT(strlen(pszSource) < uTargetCapacity, "CString::StringCopy() : ERROR source string is longer than target capacity (%d,%d) [%s]\n");
    strncpy_s(pszTarget, uTargetCapacity, pszSource, uTargetCapacity - 1);
    pszTarget[uTargetCapacity - 1] = '\n';
}

