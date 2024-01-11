#include "pch.hpp"

#include "application.hpp"

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main()
{
	aito::Logger::init();
	
	aito::Application application{};
	
	
	return 0;
}

