#include <iostream>
#include "Kaizen.h"

int main() 
{
	std::unique_ptr<Kaizen> kaizen = std::make_unique<Kaizen>();
	kaizen->Run();
}