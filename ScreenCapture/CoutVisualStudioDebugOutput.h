#pragma once
#include <sstream>

class debugstreambuffer : public std::stringbuf
{
public:
	virtual int sync()
	{
		OutputDebugStringA(str().c_str());
		return 0;
	}
};

class CoutVisualStudioDebugOutput
{
	debugstreambuffer debug_stream;
	std::streambuf* default_stream;

public:
	CoutVisualStudioDebugOutput()
	{
		default_stream = std::cout.rdbuf(&debug_stream);
	}
	~CoutVisualStudioDebugOutput()
	{
		std::cout.rdbuf(default_stream);
	}
};