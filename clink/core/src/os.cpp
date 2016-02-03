// Copyright (c) 2015 Martin Ridgers
// License: http://opensource.org/licenses/MIT

#include "pch.h"
#include "os.h"
#include "path.h"
#include "str.h"

//------------------------------------------------------------------------------
int os::get_path_type(const char* path)
{
    wstr<MAX_PATH> wpath(path);
    DWORD attr = GetFileAttributesW(wpath.c_str());
    if (attr == ~0)
        return path_type_invalid;

    if (attr & FILE_ATTRIBUTE_DIRECTORY)
        return path_type_dir;

    return path_type_file;
}

//------------------------------------------------------------------------------
void os::get_current_dir(str_base& out)
{
    wstr<MAX_PATH> wdir;
    GetCurrentDirectoryW(wdir.size(), wdir.data());
    out = wdir.c_str();
}

//------------------------------------------------------------------------------
bool os::set_current_dir(const char* dir)
{
    wstr<MAX_PATH> wdir(dir);
    return (SetCurrentDirectoryW(wdir.c_str()) == TRUE);
}

//------------------------------------------------------------------------------
bool os::make_dir(const char* dir)
{
    int type = get_path_type(dir);
    if (type == path_type_dir)
        return true;

    str<> next;
    path::get_directory(dir, next);

    if (!next.empty() && !path::is_root(next.c_str()))
        if (!make_dir(next.c_str()))
            return false;

    if (*dir)
    {
        wstr<MAX_PATH> wdir(dir);
        return (CreateDirectoryW(wdir.c_str(), nullptr) == TRUE);
    }

    return true;
}

//------------------------------------------------------------------------------
bool os::remove_dir(const char* dir)
{
    wstr<MAX_PATH> wdir(dir);
    return (RemoveDirectoryW(wdir.c_str()) == TRUE);
}

//------------------------------------------------------------------------------
bool os::unlink(const char* path)
{
    wstr<MAX_PATH> wpath(path);
    return (DeleteFileW(wpath.c_str()) == TRUE);
}

//------------------------------------------------------------------------------
bool os::move(const char* src_path, const char* dest_path)
{
    wstr<MAX_PATH> wsrc_path(src_path);
    wstr<MAX_PATH> wdest_path(dest_path);
    return (MoveFileW(wsrc_path.c_str(), wdest_path.c_str()) == TRUE);
}

//------------------------------------------------------------------------------
bool os::copy(const char* src_path, const char* dest_path)
{
    wstr<MAX_PATH> wsrc_path(src_path);
    wstr<MAX_PATH> wdest_path(dest_path);
    return (CopyFileW(wsrc_path.c_str(), wdest_path.c_str(), FALSE) == TRUE);
}

//------------------------------------------------------------------------------
bool os::get_temp_dir(str_base& out)
{
    return get_env("tmp", out) || get_env("temp", out);
}

//------------------------------------------------------------------------------
bool os::get_env(const char* name, str_base& out)
{
    wstr<32> wname(name);

    int len = GetEnvironmentVariableW(wname.c_str(), nullptr, 0);
    if (!len)
        return false;

    wstr<> wvalue;
    wvalue.reserve(len);
    len = GetEnvironmentVariableW(wname.c_str(), wvalue.data(), wvalue.size());

    out.reserve(len);
    out = wvalue.c_str();
    return true;
}