#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string> 
#include <sstream>
#include <stdio.h>
#include <windows.h>
#include <winnt.h>
#include <WinUser.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "pe3264.h"
#include "pe_exception.h"

float fileEnthropy(const std::string& file_path)
{
	float enthropy = 0.f;

	std::ifstream file;

	file.open(file_path, std::ifstream::in | std::ios::binary);

	if (file.is_open())
	{
		int statistics[256] = { 0 };

		int ch = file.get();

		while (file.good())
		{
			statistics[ch]++;
			ch = file.get();
		}

		int total = 0;
		for (int i = 0; i < 256; ++i)
		{
			total += statistics[i];
		}

		for (int i = 0; i < 256; ++i)
		{
			enthropy -= statistics[i] * log(((float)statistics[i]) / total);
		}
		enthropy /= log(2);
	}
	else
	{
		std::cout << "Can not open file " << file_path << '\n';
	}

	file.close();

	return enthropy;
}

void fileDllInfo(const std::string& file_path_exe)
{
	std::ifstream pefile;

	pefile.open(file_path_exe, std::ios::in | std::ios::binary);
	if (!pefile.is_open())
	{
		std::cout << "Can't open file" << std::endl;
		return;
	}

	try
	{
		pe32 executable(pefile);

		if (executable.has_imports())
		{
			const IMAGE_IMPORT_DESCRIPTOR* import_descriptor_array = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(executable.section_data_from_rva(executable.directory_rva(IMAGE_DIRECTORY_ENTRY_IMPORT)));

			while (import_descriptor_array->Characteristics)
			{
				std::cout << "DLL Name: " << executable.section_data_from_rva(import_descriptor_array->Name) << '\n';
				std::cout << "Import TimeDateStamp: " << import_descriptor_array->TimeDateStamp << '\n';

				const DWORD* import_address_table = reinterpret_cast<const DWORD*>(executable.section_data_from_rva(import_descriptor_array->FirstThunk));

				const DWORD* import_lookup_table = import_descriptor_array->OriginalFirstThunk == 0 ? import_address_table : reinterpret_cast<const DWORD*>(executable.section_data_from_rva(import_descriptor_array->OriginalFirstThunk));

				DWORD address_table = import_descriptor_array->FirstThunk;

				std::string name;
				WORD hint;

				std::cout << std::endl << " hint | name/ordinal                |  address" << std::endl;

				if (import_lookup_table != 0 && import_address_table != 0)
				{
					while (true)
					{
						DWORD address = *import_address_table++;

						if (!address)
							break;

						DWORD lookup = *import_lookup_table++;

						if (IMAGE_SNAP_BY_ORDINAL32(lookup))
						{
							std::stringstream stream;
							stream << "#" << IMAGE_ORDINAL32(lookup);
							name = stream.str();
							hint = 0;
						}
						else
						{
							name = executable.section_data_from_rva(lookup + 2);
							hint = *reinterpret_cast<const WORD*>(executable.section_data_from_rva(lookup));
						}

						std::cout << std::dec << "[" << std::setfill('0') << std::setw(4) << hint << "]"
							<< " " << std::left << std::setfill(' ') << std::setw(30) << name
							<< ":0x" << std::hex << std::right << std::setfill('0') << std::setw(8) << address_table
							<< std::endl;

						address_table += 4;
					}
				}

				std::cout << "==========" << std::endl << std::endl;

				import_descriptor_array++;
			}
		}
	}
	catch (const pe_exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}

}


#define RESOURSE_ID 128

void changeExeIcon(const char* exe_file_path, const char* icon_file_path)
{
	HGLOBAL hResLoad;   
	HMODULE hExe;       
	HRSRC hRes;         
	HANDLE hUpdateRes;  
	LPVOID lpResLock;  
	BOOL result;

	hExe = LoadLibrary(exe_file_path);
	if (hExe == NULL)
	{
		return;
	}

	hRes = FindResource(hExe, MAKEINTRESOURCE(RESOURSE_ID), RT_GROUP_ICON);
	if (hRes == NULL)
	{
		return;
	}

	hResLoad = LoadResource(hExe, hRes);
	if (hResLoad == NULL)
	{
		return;
	}

	lpResLock = LockResource(hResLoad);
	if (lpResLock == NULL)
	{
		return;
	}

	hUpdateRes = BeginUpdateResource(icon_file_path, FALSE);
	if (hUpdateRes == NULL)
	{
		return;
	}

	result = UpdateResource(hUpdateRes,    
		RT_GROUP_ICON,                        
		MAKEINTRESOURCE(RESOURSE_ID),         
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), 
		lpResLock,                        
		SizeofResource(hExe, hRes));      

	if (result == FALSE)
	{
		return;
	}

}
