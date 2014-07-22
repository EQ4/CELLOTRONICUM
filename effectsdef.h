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
		
		void free() {delete slider; SDL_DestroyTexture(nameTex);}
	};
	
	struct ParamBus
	{
		Bus* bus;
		int param;
		SDL_Texture* nameTex=NULL;
		ParamBus(Bus* b, int p, const char* text): bus(b), param(p) {nameTex=generateText(text);}
		
		void free() {delete bus; SDL_DestroyTexture(nameTex);}
	};
	
	struct ParamText
	{
		int posX, posY;
		int param;
		SDL_Texture* nameTex=NULL;
		ParamText(int X, int Y, int p, const char* text): posX(X), posY(Y), param(p) {nameTex=generateText(text);}
		
		void free() {SDL_DestroyTexture(nameTex);}
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
		
		
		int posX, posY;
		
		int width, height;
			
		int handlePosX, handlePosY;
		
		bool focus;
		
		SDL_Texture* nameTex;
		
		public:
		
		virtual ArgVis* getArgumentVisuals()=0;
		
		void initGUI(int X, int Y)
		{
			posX=X;
			posY=Y;
			EffectArgument* args=getAgrs();
			int argsCount=getAgrsCount();
			
			height=top_padding+bottom_padding+Bus::size;
			
			int x=width=slider_period+Bus::size+slider_period;
			
			ArgVis* argvis=getArgumentVisuals();
			
			int bus_y=top_padding;
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
						height=top_padding+bottom_padding+slider_height;
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
		
		void receiveClick(int X, int Y, bool begin)
		{
			for(int i=0;i<sliders.size();++i)
			{
				if(sliders[i].slider->receiveClick(X, Y, begin)) setAndSendArgument(sliders[i].param, sliders[i].slider->getValue());
			}
			
			int lastClicked=Bus::lastClicked;
			for(int i=0;i<buses.size();++i)
			{
				if(buses[i].bus->receiveClick(X, Y, begin))
				{
					auto pair=getLastConnection();
					Bus *bus1=pair.first, *bus2=pair.second;
					bus1->getEffect()->moveBefore(bus2->getEffect());
					int freebuf=OSCConn::getFreeBus();
					
					bus1->getEffect()->setArgument(bus1->getArg(), freebuf);
					bus2->getEffect()->setArgument(bus2->getArg(), freebuf);
					fprintf(stderr, "Connected two buses\n");
					
					break;
				}
			}
		}
		
		void receiveSecondClick(int X, int Y, bool begin)
		{
			if(begin)
			{
				if(posX<=X && X<=posX+width && posY<=Y && Y<=posY+height)
				{
					handlePosX=X-posX;
					handlePosY=Y-posY;
					focus=true;
				}
				else
				focus=false;
			}
			else
			if(focus)
			setPos(X-handlePosX, Y-handlePosY);
		}
		
		~EffectAutoGUI()
		{
			for(int i=0;i<sliders.size();++i)
			{
				sliders[i].free();
			}
			SDL_DestroyTexture(nameTex);
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
			
			Distecho(int X, int Y, int inbus=8, int outbus=0): 
			args({EffectArgument("inbus", inbus), EffectArgument("outbus", outbus), EffectArgument("decay", 2.0f), EffectArgument("amp", 1.0f), EffectArgument("delay", 5.0f)}),
			argsVis({ArgVis(VT_INBUS), ArgVis(VT_OUTBUS), ArgVis(VT_SLIDER, 0.0f, 5.0f), ArgVis(VT_SLIDER, 0.0f, 2.5f), ArgVis(VT_SLIDER, 0.0f, 15.0f)})
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
			
			Playbuf(int X, int Y, int bufnum, int outbus=0): args({EffectArgument("bufnum", bufnum), EffectArgument("outbus", outbus)}),
			argsVis({ArgVis(VT_TEXT, std::string(OSCConn::getBufferFileById(bufnum))), ArgVis(VT_OUTBUS)})
			{sendInstance(); initGUI(X, Y);}
	};
	
	void registerEffects();
	
#endif
