#include "World01.h"
#include "World02.h"
#include "World03.h"
#include "World04.h"
#include "World05.h"
#include "World06.h"
#include "World07.h"
#include "World08.h"
#include "Core/Core.h"
#include "Framework/Framework.h"
#include "Renderer/Renderer.h"

// OpenGL is right-handed - positive Z comes out of the screen 

using namespace std;

int main(int argc, char* argv[])
{
	INFO_LOG("Motor Initialisieren, bruh...")


	// StringUtils tests
	//std::cout << "ToUpper: " << nc::StringUtils::ToUpper("i was small but now i'm big") << std::endl;

	//std::cout << "ToLower: " << nc::StringUtils::ToLower("I WAS BIG BUT NOW I'M SMALL") << std::endl;

	//std::cout << "IsEqualIgnoreCase: " << (nc::StringUtils::IsEqualIgnoreCase("maple", "MAPLE") ? "Equal" : "Not Equal") << std::endl;
	//std::cout << "IsEqualIgnoreCase: " << (nc::StringUtils::IsEqualIgnoreCase("Neumont", "UofU") ? "Equal" : "Not Equal") << std::endl;

	//std::cout << "CreateUnique: " << nc::StringUtils::CreateUnique("id_") << std::endl;
	//std::cout << "CreateUnique: " << nc::StringUtils::CreateUnique("id_") << std::endl;
	//std::cout << "CreateUnique: " << nc::StringUtils::CreateUnique("id_") << std::endl;

	//nc::MemoryTracker::Initialize();
	nc::seedRandom((unsigned int)time(nullptr));
	nc::setFilePath("assets");

	ENGINE.Initialize();

	auto world = make_unique<nc::World08>();
	world->Initialize();

	// main loop
	bool quit = false;
	while (!quit)
	{
		// update
		ENGINE.Update();
		quit = ENGINE.IsQuit();

		world->Update(ENGINE.GetTime().GetDeltaTime());

		// draw
		world->Draw(*ENGINE.GetSystem<nc::Renderer>());
	}

	world->Shutdown();
	ENGINE.Shutdown();

	return 0;
}
