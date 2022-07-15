#include<future>

#include"FileInfoFuncs.h"

int main(int argc, const char* argv[])
{
	std::string filename_exe = "C:/Users/Sculptor/source/repos/prc++/Debug/prc++.exe";
	std::string filename_ico = "C:/Users/Sculptor/source/repos/win_api_pr/Ico/test_ico.ico";

	std::cout << "Exe file path: ";
	std::cin >> filename_exe;

	std::cout << "\nIco file path: ";
	std::cin >> filename_ico;

	std::cout << "\n\n";

	fileDllInfo(filename_exe);

	const float exe_enthropy = fileEnthropy(filename_exe);
	const float ico_enthropy = fileEnthropy(filename_ico);

	std::cout << "File Enthropy: " << exe_enthropy << "    \t\t\t" + filename_exe << "\n\n";
	std::cout << "File Enthropy: " << ico_enthropy << "    \t\t\t" + filename_ico << "\n\n";

	changeExeIcon(filename_exe.c_str(), filename_ico.c_str());

	return 0;
}

