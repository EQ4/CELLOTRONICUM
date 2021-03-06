#include <cstdio>
#include <cstring>
#include <chrono>
#include <thread>
#ifdef  __LINUX__
	#include <unistd.h>
#endif
#include "osc.h"
#include "effects.h"
#include "effectsdef.h"
#include "graphics.h"
#include "controllersdef.h"

EffectCreator effectCreator;

///czeka podaną liczbę milisekund
void waitFor(int ms) 
{
	std::chrono::milliseconds dura(ms);
	std::this_thread::sleep_for(dura);
}

///bufor do przechowania stringu z nazwą pliku
char fileStr[MAX_PATH];

///sprawdza sygnały z myszy i klawiatury i wykonuje odpowiednie rzeczy
bool checkInputs()
{
	bool quit = false;
	
	auto effectInstanceList=getEffectInstanceList();
	
	auto controllerInstanceList=getControllerInstanceList();

	while (SDL_PollEvent(&event))
	{
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		switch(event.type)
		{
			case SDL_QUIT:
				quit = true;
			break;
			case SDL_MOUSEBUTTONDOWN:
				{
					int x=event.button.x;
					int y=event.button.y;
					if(event.button.button==SDL_BUTTON_LEFT && !(state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) && !(state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]))
					{
						for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();++it)
						{
							if(it->second->receiveClick(x, y, ME_PRESS))break;
						}
						
						for(auto it=controllerInstanceList->rbegin();it!=controllerInstanceList->rend();++it)
						{
							if(it->second->receiveClick(x, y, ME_PRESS))break;
						}
					}
					else
					if(event.button.button==SDL_BUTTON_RIGHT || (event.button.button==SDL_BUTTON_LEFT && (state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT])))
					{
						for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();++it)
						{
							if(it->second->receiveSecondClick(x, y, ME_PRESS))break;
						}
						
						for(auto it=controllerInstanceList->rbegin();it!=controllerInstanceList->rend();++it)
						{
							if(it->second->receiveSecondClick(x, y, ME_PRESS))break;
						}
					}
					else
					if(event.button.button==SDL_BUTTON_MIDDLE || (event.button.button==SDL_BUTTON_LEFT && (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL])))
					{
						for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();)
						{
							auto it2=it;
							++it;
							if(it2->second->receiveThridClick(x, y, ME_PRESS))break;
						}
						
						for(auto it=controllerInstanceList->rbegin();it!=controllerInstanceList->rend();++it)
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
					if(event.button.button==SDL_BUTTON_LEFT && !(state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) && !(state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]))
					{
						for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();++it)
						{
							it->second->receiveClick(x, y, ME_RELEASE);
						}
						for(auto it=controllerInstanceList->rbegin();it!=controllerInstanceList->rend();++it)
						{
							it->second->receiveClick(x, y, ME_RELEASE);
						}
					}
					else
					if(event.button.button==SDL_BUTTON_RIGHT || (event.button.button==SDL_BUTTON_LEFT && (state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT])))
					{
						for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();++it)
						{
							it->second->receiveSecondClick(x, y, ME_RELEASE);
						}
						for(auto it=controllerInstanceList->rbegin();it!=controllerInstanceList->rend();++it)
						{
							it->second->receiveSecondClick(x, y, ME_RELEASE);
						}
					}
					else
					if(event.button.button==SDL_BUTTON_MIDDLE || (event.button.button==SDL_BUTTON_LEFT && (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL])))
					{
						for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();)
						{
							auto it2=it;
							++it;
							it2->second->receiveThridClick(x, y, ME_RELEASE);
						}
						for(auto it=controllerInstanceList->rbegin();it!=controllerInstanceList->rend();++it)
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
					if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && !(state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) && !(state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]))
					{
						for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();++it)
						{
							if(it->second->receiveClick(x, y, ME_REPEAT))break;
						}
						for(auto it=controllerInstanceList->rbegin();it!=controllerInstanceList->rend();++it)
						{
							if(it->second->receiveClick(x, y, ME_REPEAT))break;
						}
					}
					else
					if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT) || (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && (state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT])))
					{
						for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();++it)
						{
							if(it->second->receiveSecondClick(x, y, ME_REPEAT))break;
						}
						for(auto it=controllerInstanceList->rbegin();it!=controllerInstanceList->rend();++it)
						{
							if(it->second->receiveSecondClick(x, y, ME_REPEAT))break;
						}
					}
					else
					if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE) || (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL])))
					{
						for(auto it=effectInstanceList->rbegin();it!=effectInstanceList->rend();)
						{
							auto it2=it;
							++it;
							if(it2->second->receiveThridClick(x, y, ME_REPEAT))break;
						}
						for(auto it=controllerInstanceList->rbegin();it!=controllerInstanceList->rend();++it)
						{
							auto it2=it;
							++it;
							if(it2->second->receiveThridClick(x, y, ME_REPEAT))break;
						}
					}
				}
			break;
			case SDL_KEYDOWN:
				
				auto it=effectInstanceList->begin();
				for(;it!=effectInstanceList->end();++it)
				{
					if(it->second->receiveKeyboardEvent(event.key.keysym.scancode))break;
				}
				
				if(it==effectInstanceList->end())
				effectCreator.receiveKeyboardEvent(event.key.keysym.scancode);
			
				if((state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) && state[SDL_SCANCODE_S])
				{
					getSaveFile(fileStr, MAX_PATH);
					Effect::saveToFile(fileStr);
				}
				else if((state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) && state[SDL_SCANCODE_L])
				{
					Effect::saveToFile("session_before_load.cello");
					Effect::clearAll();
					getOpenFile(fileStr, MAX_PATH);
					Effect::loadFromFile(fileStr);
				}
				else if((state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) && (state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) && state[SDL_SCANCODE_E])
				{
					Effect::saveToFile("session_before_erase.cello");
					Effect::clearAll();
				}
				else if((state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) && state[SDL_SCANCODE_R])
				{
					OSCConn::recordToFile();
				}
				else if(!(state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) && (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) && state[SDL_SCANCODE_R])
				{
					OSCConn::startRecord();
				}
				else if((state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) && state[SDL_SCANCODE_R])
				{
					OSCConn::stopRecord();
				}
				
			break;
		}
	}
	
	return quit;
}

///sprawdza czy plik intnieje
inline bool existsTest(const char* name) 
{
    if(FILE *file = fopen(name, "r")) 
	{
        fclose(file);
        return true;
    } 
	else
    return false;
}

const char* SC_PATH_FILE="scpath.txt"; ///Nazwa pliku ze ścieżką do sclang

const char* SC_MAIN_FILE="sc/main.scd"; ///nazwa pliku z głównym plikiem SC

char sclangPath[MAX_PATH]; ///ścieżka do sclang

///Uruchamia interpreter (NIE server) SuperCollidera
bool launchSuperCollider()
{
	char currentDir[MAX_PATH];
	getCurrentDir(currentDir, MAX_PATH);
	
	char sclangPathDirectory[MAX_PATH];
	
	for(int i=strlen(sclangPath)-1;i>=1;--i)
	{
		if((sclangPath[i]=='/' || sclangPath[i]=='\\') && sclangPath[i-1]!='\\')
		{
			strncpy(sclangPathDirectory, sclangPath, i);
			sclangPathDirectory[i]='\0';
			break;
		}
		if(i==1) return false;
	}
	
	char main_scd_path[MAX_PATH+10];
	#ifdef __LINUX__
		char dir_sep_char = '/';
	#else
		char dir_sep_char = '\\';
	#endif
	sprintf(main_scd_path, "%s%c%s", currentDir, dir_sep_char, SC_MAIN_FILE);
	
	
	
	#ifdef __LINUX__
		if(!fork()){
			printf("Executing: %s %s...\n", sclangPath, main_scd_path);
			//execlp(sclangPath,sclangPath,"-d","/usr/share/SuperCollider",main_scd_path,NULL);
			execlp(sclangPath,sclangPath,main_scd_path,NULL);
			// execlp never returns
		}

	#else
		char tab[MAX_PATH*2+10];
		sprintf(tab, "-d \"%s\" \"%s\"", sclangPathDirectory, main_scd_path);

		//przykładowe polecenie: "C:\Program Files (x86)\SuperCollider-3.6.6\sclang.exe" -d "C:\Program Files (x86)\SuperCollider-3.6.6" "C:\Users\praktykant\CELLOTRONICUM\main.scd"
		printf("Executing: %s %s...\n", sclangPath, tab);
		ShellExecute(NULL, "open", sclangPath, tab, NULL, SW_SHOWDEFAULT);
	#endif
	

	return true;
}

///zapisuje ścieżkę do sclang do pliku
void saveSCPath()
{
	FILE* pathFile=fopen(SC_PATH_FILE, "w");
	fprintf(pathFile, "%s", sclangPath);
	fclose(pathFile);
}

///Daje użytkownikowi możliwość wprowadzenia ścieżki do sclang
bool getSCPath()
{
	FILE* pathFile=fopen(SC_PATH_FILE, "r");
	if(pathFile!=NULL)
	{
		fgets(sclangPath, MAX_PATH, pathFile);
		if(existsTest(sclangPath))
		{
			fclose(pathFile);
			return true;
		}
	}

#ifdef __LINUX__
	if(existsTest("/usr/bin/sclang")){
		strcpy(sclangPath,"/usr/bin/sclang");
		return true;
	}
#endif
	
	ShowAlert(L"SuperCollider Path", L"To launch program you must provide path to sclang in SuperCollider folder.");
	getOpenFile(sclangPath, MAX_PATH);
	
	printf("Selected path: %s\n", sclangPath);
	
	if(existsTest(sclangPath))
	{
		return true;
	}
	else
	{
		ShowAlert(L"SuperCollider Path", L"Patch to sclang is incorrect!");
		return false;
	}
}

SDL_Texture* recordingInfoTex;

void drawRecordingInfo()
{
	if((SDL_GetTicks()/800)%2 && OSCConn::isRecording())
	drawTexture(recordingInfoTex, 5, 0);
}

#undef main //hack żeby działało na windowsie

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
	else
	{
		if(getSCPath() && launchSuperCollider());
		else
		{
			fprintf(stderr, "Cant get sclang patch - exiting\n");
			exit(0);
		}
	}
	
	registerEffects();
	registerControllers();
	
	if(!OSCConn::connect()) exit(1);
	
	if(!OSCConn::startServer()) exit(2);
	
	if(!checkEffectsList()) {OSCConn::quitServer(); exit(3);}
	
	saveSCPath();
	
	initSDL();
	
	effectCreator.init();
	
	recordingInfoTex=generateText("Recording", COLOR_SELECTED_TEXT);
	
	auto effectInstanceList=getEffectInstanceList();
	
	auto controllerInstanceList=getControllerInstanceList();
	
	
	while (!checkInputs())
	{
		for(auto it=controllerInstanceList->rbegin();it!=controllerInstanceList->rend();++it)
		{
			it->second->step();
		}
		
		setColor(COLOR_CLEARCOLOR);
		SDL_RenderClear(render);
		drawRecordingInfo();
		
		for(auto it=effectInstanceList->begin();it!=effectInstanceList->end();++it)
		{
			it->second->doSomething();
		}
		
		for(auto it=effectInstanceList->begin();it!=effectInstanceList->end();++it)
		{
			it->second->draw();
		}
		
		for(auto it=controllerInstanceList->rbegin();it!=controllerInstanceList->rend();++it)
		{
			it->second->draw();
		}

		drawConnections();
		
		int screen_width;
		SDL_GetWindowSize(window, &screen_width, NULL);

		effectCreator.draw(screen_width-EffectCreator::menu_period, 0);
		
		SDL_RenderPresent(render);
	}
	
	Effect::saveToFile("last_session.cello");
	
	Effect::clearAll();
	
	quitSDL();
	
	OSCConn::quitServer();
	fprintf(stderr, "Done\n");
}
