#ifndef EFFECTSDEF_H
#define EFFECTSDEF_H
#include "effects.h"
#include "graphics.h"
	
	
	struct ParamSlider
	{
		Slider* slider;
		int param;
		SDL_Texture* nameTex=NULL;
		ParamSlider(Slider* s, int p, const char* text): slider(s), param(p) {nameTex=generateText(text);}
		
		void free() {delete slider; SDL_DestroyTexture(nameTex); nameTex=NULL;}
	};
	
	struct ParamBus
	{
		Bus* bus;
		int param;
		SDL_Texture* nameTex=NULL;
		ParamBus(Bus* b, int p, const char* text): bus(b), param(p) {nameTex=generateText(text);}
		
		void free() {delete bus; SDL_DestroyTexture(nameTex); nameTex=NULL;} 
	};
	
	struct ParamText
	{
		int posX, posY;
		int param;
		SDL_Texture* nameTex=NULL;
		ParamText(int X, int Y, int p, const char* text): posX(X), posY(Y), param(p) {nameTex=generateText(text);}
		void free() {SDL_DestroyTexture(nameTex); nameTex=NULL;}
	};
	
	enum VisulalisationType{
            VT_INBUS,
            VT_OUTBUS,
            VT_SLIDER,
			VT_TEXT
    };
	
    struct ArgVis
    {
        VisulalisationType visType;
        void* data=NULL;
        
        ArgVis(VisulalisationType type, float min, float max)
        {
            visType=VT_SLIDER;
            data=new float[2];
			((float*)data)[0]=min;
			((float*)data)[1]=max;
        }
		ArgVis(VisulalisationType type, std::string str)
        {
            visType=VT_TEXT;
            data=new std::string(str);
        }
        ArgVis(VisulalisationType type)
        {
            visType=type;
        }
        ~ArgVis()
        {
            if(visType==VT_SLIDER)
            delete [] (float*)data;
			else
			if(visType==VT_TEXT)
			delete (std::string*)data;
        }
        
    };
    
	class EffectAutoGUI : public Effect
	{
		static constexpr int slider_width=30;
		static constexpr int slider_period=20;
		static constexpr int slider_height=160;
		static constexpr int top_padding=35;
		static constexpr int bottom_padding=30;
		static constexpr int bus_period=35;
		
		std::vector <ParamSlider> sliders;
		std::vector <ParamBus> buses;
		std::vector <ParamText> texts;
		
		Button* pauseButton;
		
		int posX, posY;
		
		int width, height;
			
		int handlePosX, handlePosY;
		
		bool focus=false;
		
		SDL_Texture* nameTex;
		
		public:
		
		virtual ArgVis* getArgumentVisuals()=0;
		
		void initGUI(int X, int Y)
		{
			posX=X;
			posY=Y;
			
			pauseButton=new Button(X, Y, 0);
			
			EffectArgument* args=getAgrs();
			int argsCount=getAgrsCount();
			
			int x=width=slider_period+Bus::size+slider_period;
			
			ArgVis* argvis=getArgumentVisuals();
			
			int bus_y=top_padding;
			int slider_y=0;
			int bus_y2=top_padding;
			
			for(int i=0;i<argsCount;++i)
			{
				switch(argvis[i].visType)
				{
					case VT_INBUS:
					buses.push_back(ParamBus(new Bus(posX+slider_period, posY+bus_y, BT_INBUS, this, i), i, args[i].getName()));
					bus_y+=bus_period;
					break;
					case VT_OUTBUS:
					buses.push_back(ParamBus(new Bus(posX, posY+bus_y2, BT_OUTBUS, this, i), i, args[i].getName()));
					bus_y2+=bus_period;
					break;
					case VT_SLIDER:
					{
						slider_y=slider_height;
						float min=((float*)argvis[i].data)[0];
						float max=((float*)argvis[i].data)[1];
						sliders.push_back(ParamSlider(new Slider(posX+x, posY+top_padding, slider_width, slider_height, min, max, args[i].getFloatValue()), i, args[i].getName()));
						x+=slider_width+slider_period;
					}
					break;
					case VT_TEXT:
						texts.push_back(ParamText(posX+slider_period, posY+bus_y, i, ((std::string*)argvis[i].data)->c_str()));
						int w;
						SDL_QueryTexture(texts.back().nameTex, NULL, NULL, &w, NULL);
						x=std::max(slider_period+w+slider_period, x);
					break;
				}
			}
			
			height=bottom_padding+std::max(std::max(bus_y, bus_y2), top_padding+slider_y);
			
			width=x;
			
			for(int i=0;i<buses.size();++i)
			{
				if(buses[i].bus->getType()==BT_OUTBUS)
				{
					buses[i].bus->setPos(posX+x, buses[i].bus->getPosY());
				}
			}
			
			width+=Bus::size+slider_period;
			
			nameTex=generateText(getName());
		}
		
		void setPos(int X, int Y)
		{
			pauseButton->move(X-posX, Y-posY);
			
			for(int i=0;i<sliders.size();++i)
			{
				sliders[i].slider->move(X-posX, Y-posY);
			}
			for(int i=0;i<buses.size();++i)
			{
				buses[i].bus->move(X-posX, Y-posY);
			}
			for(int i=0;i<texts.size();++i)
			{
				texts[i].posX+=X-posX;
				texts[i].posY+=Y-posY;
			}
			posX=X; posY=Y;
		}
		
		void draw()
		{
			SDL_Rect rect;
			rect.x = posX; rect.y = posY;
			rect.w = width;
			rect.h = height;
			SDL_SetRenderDrawColor(render, 220, 220, 220, 255);
			SDL_RenderFillRect(render, &rect);
			SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
			SDL_RenderDrawRect(render, &rect);
			
			int w, h;
			SDL_QueryTexture(nameTex, NULL, NULL, &w, &h);
			
			SDL_Rect nameRect;
			nameRect.y=posY+2;
			nameRect.x=posX+width/2-w/2;
			nameRect.w=w;
			nameRect.h=h;
			
			SDL_RenderCopy(render, nameTex, NULL, &nameRect);
			
			
			pauseButton->setSymbol(int(isPaused()));
			pauseButton->draw();
			
			for(int i=0;i<sliders.size();++i)
			{
				sliders[i].slider->draw();
				int w, h;
				SDL_QueryTexture(sliders[i].nameTex, NULL, NULL, &w, &h);
			
				nameRect.y=sliders[i].slider->getPosY()+slider_height;
				nameRect.x=sliders[i].slider->getPosX()+slider_width/2-w/2;
				nameRect.w=w;
				nameRect.h=h;		
				
				SDL_RenderCopy(render, sliders[i].nameTex, NULL, &nameRect);
			}
			
			for(int i=0;i<buses.size();++i)
			{
				buses[i].bus->draw();
				int w, h;
				SDL_QueryTexture(buses[i].nameTex, NULL, NULL, &w, &h);
			
				nameRect.y=buses[i].bus->getPosY()+Bus::size;
				nameRect.x=buses[i].bus->getPosX()+Bus::size/2-w/2;
				nameRect.w=w;
				nameRect.h=h;		
				
				SDL_RenderCopy(render, buses[i].nameTex, NULL, &nameRect);
			}
			for(int i=0;i<texts.size();++i)
			{
				int w, h;
				SDL_QueryTexture(texts[i].nameTex, NULL, NULL, &w, &h);
			
				nameRect.y=texts[i].posY;
				nameRect.x=texts[i].posX;
				nameRect.w=w;
				nameRect.h=h;		
				
				SDL_RenderCopy(render, texts[i].nameTex, NULL, &nameRect);
			}
			
		}
		
		bool receiveClick(int X, int Y, MouseEvent me)
		{
			if(!(posX<=X && X<=posX+width && posY<=Y && Y<=posY+height)) return false;
			if(pauseButton->receiveClick(X, Y, me)) 
			{
				paused=!paused;
				if(paused)
					pauseInstance();
				else
					unpauseInstance();
			}
			for(int i=0;i<sliders.size();++i)
			{
				if(sliders[i].slider->receiveClick(X, Y, me)) setAndSendArgument(sliders[i].param, sliders[i].slider->getValue());
			}
			
			int lastClicked=Bus::lastClicked;
			for(int i=0;i<buses.size();++i)
			{
				if(buses[i].bus->receiveClick(X, Y, me))
				{
					auto pair=getLastConnection();
					Bus *bus1=pair.first, *bus2=pair.second;
					
					if(bus1==NULL)
					{
						bus2->getEffect()->getAgrs()[bus2->getArg()].set(OSCConn::getFreeBus());
					}
					else
					{
						updateTopologicalSequence();
						
						int freebus=bus1->getEffect()->getAgrs()[bus1->getArg()].getIntValue();
						
						OSCConn::deleteBus(bus2->getEffect()->getAgrs()[bus2->getArg()].getIntValue());
						
						bus2->getEffect()->getAgrs()[bus2->getArg()].set(freebus);
						
						bus2->getEffect()->setAndSendArgument(bus2->getArg(), freebus);
						fprintf(stderr, "Connected two buses to %d\n", freebus);
					}
					break;
				}
			}
			return true;
		}
		
		bool receiveSecondClick(int X, int Y, MouseEvent me)
		{
			if(me==ME_PRESS)
			{
				if(posX<=X && X<=posX+width && posY<=Y && Y<=posY+height)
				{
					handlePosX=X-posX;
					handlePosY=Y-posY;
					focus=true;
					return true;
				}
				else
				focus=false;
				return false;
			}
			else if(me==ME_REPEAT && focus)
			{
				setPos(X-handlePosX, Y-handlePosY);
				return true;
			}
			else if(me==ME_RELEASE)
			{
				focus=false;
				return true;
			}
			return false;
		}
		
		bool receiveThridClick(int X, int Y, MouseEvent me)
		{
			if(posX<=X && X<=posX+width && posY<=Y && Y<=posY+height && me==ME_PRESS)
			{
				deleteInstance();
				getEffectInstanceList()->erase(id);
				for(int i=0;i<buses.size();++i)
				buses[i].bus->removeBus();
				
				for(auto it=effectTree.begin();it!=effectTree.end();)
				{
					if((*it).first==id || (*it).second==id)
					{
						it=effectTree.erase(it);
					}
					else
					++it;
				}
				
				delete this;
				return true;
			}
			else return false;
		}
		
		~EffectAutoGUI()
		{
			printf("deconstructor %p\n", this);
			for(int i=0;i<sliders.size();++i)
			sliders[i].free();
			for(int i=0;i<buses.size();++i)
			buses[i].free();
			for(int i=0;i<texts.size();++i)
			texts[i].free();
			SDL_DestroyTexture(nameTex);
			delete pauseButton;
		}
		
	};
	
	
	class Distecho : public EffectAutoGUI
	{
		private:
		
			static const int argsCount=5;
			EffectArgument args[argsCount];
			
			ArgVis argsVis[argsCount];
			
		public:
			static constexpr const char* name="eff_distecho";
			const char* getName() {return name;}
			EffectArgument* getAgrs() {return args;}
			const int getAgrsCount() {return argsCount;}
			ArgVis* getArgumentVisuals() {return argsVis;}
			
			Distecho(int X, int Y): 
			args({EffectArgument("inbus", OSCConn::getFreeBus()), EffectArgument("outbus", OSCConn::getFreeBus()), EffectArgument("decay", 2.0f), EffectArgument("amp", 1.0f), EffectArgument("delay", 0.15f)}),
			argsVis({ArgVis(VT_INBUS), ArgVis(VT_OUTBUS), ArgVis(VT_SLIDER, 0.0f, 5.0f), ArgVis(VT_SLIDER, 0.0f, 2.5f), ArgVis(VT_SLIDER, 0.0f, 1.0f)})
			{sendInstance(); initGUI(X, Y);}
	};
	
	
	class Playbuf : public EffectAutoGUI
	{
		private:
			static const int argsCount=2;
			EffectArgument args[argsCount];
			ArgVis argsVis[argsCount];
		public:
			static constexpr const char* name="eff_playbuf";
			const char* getName() {return name;}
			EffectArgument* getAgrs() {return args;}
			const int getAgrsCount() {return argsCount;}
			ArgVis* getArgumentVisuals() {return argsVis;}
			
			Playbuf(int X, int Y, int bufnum): args({EffectArgument("bufnum", bufnum), EffectArgument("outbus", OSCConn::getFreeBus())}),
			argsVis({ArgVis(VT_TEXT, std::string(OSCConn::getBufferFileById(bufnum))), ArgVis(VT_OUTBUS)})
			{sendInstance(true); initGUI(X, Y);}
	};
	
	class Input : public EffectAutoGUI
	{
		private:
			static const int argsCount=1;
			EffectArgument args[argsCount];
			ArgVis argsVis[argsCount];
		public:
			static constexpr const char* name="eff_input";
			const char* getName() {return name;}
			EffectArgument* getAgrs() {return args;}
			const int getAgrsCount() {return argsCount;}
			ArgVis* getArgumentVisuals() {return argsVis;}
			
			Input(int X, int Y): args({EffectArgument("outbus", OSCConn::getFreeBus())}),
			argsVis({ArgVis(VT_OUTBUS)})
			{sendInstance(); initGUI(X, Y);}
	};
	
	class Output : public EffectAutoGUI
	{
		private:
			static const int argsCount=2;
			EffectArgument args[argsCount];
			ArgVis argsVis[argsCount];
		public:
			static constexpr const char* name="eff_output";
			const char* getName() {return name;}
			EffectArgument* getAgrs() {return args;}
			const int getAgrsCount() {return argsCount;}
			ArgVis* getArgumentVisuals() {return argsVis;}
			
			Output(int X, int Y): args({EffectArgument("inbus1", OSCConn::getFreeBus()), EffectArgument("inbus2", OSCConn::getFreeBus())}),
			argsVis({ArgVis(VT_INBUS), ArgVis(VT_INBUS)})
			{sendInstance(); initGUI(X, Y);}
	};
	
	void registerEffects();
	
	Effect* getEffect(const char* name, int X, int Y);
	
#endif
