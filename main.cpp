#include <cstdio>
#include <cstring>
#include <chrono>
#include <thread>
#include "osc.h"
#include "effects.h"
#include "effectsdef.h"
#include "graphics.h"

EffectCreator effectCreator;

void waitFor(int ms)
{
	std::chrono::milliseconds dura(ms);
	std::this_thread::sleep_for(dura);
}

char fileStr[MAX_PATH];


#undef main
int main (int argc, char** argv)
{
	if(argc>1)
	{
		OSCConn::setServer(argv[1]);
		if(argc>2)
		{
			OSCConn::setPort(atoi(argv[2])); 
		}
	}
	
	registerEffects();
	
	if(!OSCConn::connect()) exit(1);
	
	if(!OSCConn::startServer()) exit(2);
	
	if(!checkEffectsList()) {OSCConn::quitServer(); exit(3);}
	
	initSDL();
	
	effectCreator.init();
	
	auto effectInstanceList=getEffectInstanceList();
	
	bool quit = false;
	
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					quit = true;
				break;
				case SDL_MOUSEBUTTONDOWN:
					{
						int x=event.button.x;
						int y=event.button.y;
						if(event.button.button==SDL_BUTTON_LEFT)
						{
							for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();++it)
							{
								if(it->second->receiveClick(x, y, ME_PRESS))break;
							}
						}
						else
						if(event.button.button==SDL_BUTTON_RIGHT)
						{
							for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();++it)
							{
								if(it->second->receiveSecondClick(x, y, ME_PRESS))break;
							}
						}
						else
						if(event.button.button==SDL_BUTTON_MIDDLE)
						{
							for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();)
							{
								auto it2=it;
								++it;
								if(it2->second->receiveThridClick(x, y, ME_PRESS))break;
							}
						}
					}
				break;
				case SDL_MOUSEBUTTONUP:
					{
						int x=event.button.x;
						int y=event.button.y;
						if(event.button.button==SDL_BUTTON_LEFT)
						{
							for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();++it)
							{
								it->second->receiveClick(x, y, ME_RELEASE);
							}
						}
						else
						if(event.button.button==SDL_BUTTON_RIGHT)
						{
							for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();++it)
							{
								it->second->receiveSecondClick(x, y, ME_RELEASE);
							}
						}
						else
						if(event.button.button==SDL_BUTTON_MIDDLE)
						{
							for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();)
							{
								auto it2=it;
								++it;
								it2->second->receiveThridClick(x, y, ME_RELEASE);
							}
						}
					}
				break;
				case SDL_MOUSEMOTION:
					{
						int x=event.button.x;
						int y=event.button.y;
						if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
						{
							for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();++it)
							{
								if(it->second->receiveClick(x, y, ME_REPEAT))break;
							}
						}
						else
						if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
						{
							for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();++it)
							{
								if(it->second->receiveSecondClick(x, y, ME_REPEAT))break;
							}
						}
						else
						if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE))
						{
							for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();)
							{
								auto it2=it;
								++it;
								if(it2->second->receiveThridClick(x, y, ME_REPEAT))break;
							}
						}
					}
				break;
				case SDL_KEYDOWN:
					effectCreator.receiveKeyboardEvent(event.key.keysym.scancode);
					
					const Uint8 *state = SDL_GetKeyboardState(NULL);
				
					if(state[SDL_SCANCODE_LCTRL] && state[SDL_SCANCODE_S])
					{
						getSaveFile(fileStr, MAX_PATH);
						Effect::saveToFile(fileStr);
					}
					else if(state[SDL_SCANCODE_LCTRL] && state[SDL_SCANCODE_L])
					{
						Effect::saveToFile("session_before_load.cello");
						getOpenFile(fileStr, MAX_PATH);
						Effect::loadFromFile(fileStr);
					}
				break;
			}
		}
		
		
		SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
		SDL_RenderClear(render);
		
		for(auto it=effectInstanceList->begin();it!=effectInstanceList->end();++it)
		{
			it->second->draw();
		}
		
		drawConnections();
		effectCreator.draw(SCREEN_WIDTH-20, 0);
		
		SDL_RenderPresent(render);
	}
	
	Effect::saveToFile("last_session.cello");
	
	for(auto it=effectInstanceList->begin();it!=effectInstanceList->end();++it)
	{
		delete (*it).second;
	}
	
	quitSDL();
	
	OSCConn::quitServer();
	fprintf(stderr, "Done\n");
}