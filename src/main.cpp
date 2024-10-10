#include "pch.hpp"

#include "application.hpp"

#include <iostream>

int main()
{
	flwfrg::init();
	rayimage::Application application{1280, 720};

	application.run();

	return 0;
}