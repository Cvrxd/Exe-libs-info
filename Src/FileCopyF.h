#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <windows.h>

void copyFileWinapi  (const size_t& block_size, LPCSTR src_path, LPCSTR dest_path);                  //Copy file in WINAPI style
void copyFileCpp     (const size_t& block_size, const wchar_t* src_path, const wchar_t* dest_path);  //Copy file in C++ style
void copyFileC       (const size_t& block_size, const wchar_t* src_path, const wchar_t* dest_path);  //Copy file in C style

void copyFileC(const size_t& block_size, const wchar_t* src_path, const wchar_t* dest_path)
{
    FILE* fRead = NULL;
    FILE* fWrite = NULL;

    char* buffer = (char*)calloc(block_size + 1, sizeof(char));

    LARGE_INTEGER pos(0);

    _wfopen_s(&fRead, src_path, L"rb"); //Read in binary unicode str
    _wfopen_s(&fWrite, dest_path, L"wb"); //Write in binary unicode str

    if (fRead && fWrite)
    {
        size_t iRead;
        while (!feof(fRead))
        {
            iRead = fread_s(buffer, block_size, sizeof(char), block_size, fRead);
            fwrite(buffer, sizeof(char), iRead, fWrite);
            pos.QuadPart += iRead;
        }
    }

    if (fRead)
    {
        fclose(fRead);
    }

    if (fWrite)
    {
        fclose(fWrite);
    }

    if (buffer)
    {
        free(buffer);
    }

}

void copyFileCpp(const size_t& block_size, const wchar_t* src_path, const wchar_t* dest_path)
{
    char* buffer = new char[block_size + 1];

    std::ifstream inStream(src_path, std::ios::binary);
    std::ofstream outStream(dest_path, std::ios::binary);

    LARGE_INTEGER pos_f(0);

    if (inStream && outStream)
    {
        std::streamsize iCount;

        while (!inStream.eof())
        {
            inStream.read(buffer, block_size * sizeof(char));
            iCount = inStream.gcount();
            outStream.write(buffer, iCount * sizeof(char));

            pos_f.QuadPart += iCount;
        }
    }

    inStream.close();
    outStream.close();

    if (buffer)
        delete[] buffer;
}

void copyFileWinapi(const size_t& block_size, LPCSTR src_path, LPCSTR dest_path)
{
    HANDLE hRead = CreateFile(
        src_path,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    HANDLE hWrite = CreateFile(
        dest_path,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    char* buffer = (char*)calloc(block_size + 1, sizeof(char));

    if (hRead && hWrite)
    {
        DWORD iIn(0);
        DWORD iOut(0);

        while (ReadFile(hRead, buffer, block_size, &iIn, NULL) && iIn > 0)
        {
            WriteFile(hWrite, buffer, iIn, &iOut, NULL);

            if (iIn != iOut)
            {
                CloseHandle(hRead);
                CloseHandle(hWrite);
                free(buffer);

                throw L"Error read/write file";
            }
        }
    }

    if (hRead)
        CloseHandle(hRead);

    if (hWrite)
        CloseHandle(hWrite);

    if (buffer)
        free(buffer);
}