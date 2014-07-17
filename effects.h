#ifndef EFFECTS_H
#define EFFECTS_H
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <stdarg.h>
#include "osc.h"

	enum VarType{
		TYPE_UNKNOWN,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_STRING,
		TYPE_BLOB,
		TYPES_SIZE
	};
	
	class EffectArgument{
		friend class Effect;
		private:
		
			VarType type;
			const char* name;
			void* value;
			
			void set(int var);
			
			void set(float var);
			
			void set(std::string var);
			
			void addArgumentToMessage(Message* msg);
			
			void sendArgument(int id);	
			
		public:
		
			const char* getName() {return name;}
		
			EffectArgument(const char* n, int var): name(n) {set(var);}
			EffectArgument(const char* n, float var): name(n) {set(var);}
			EffectArgument(const char* n, std::string var): name(n) {set(var);}
			
			~EffectArgument();
	};
	
	class Effect{
	
		private:
			int id;
			static int lastId;		
		public:
			void sendInstance();
			void deleteInstance();
			void moveBefore(Effect* effect);
		
			Effect();
			
			~Effect();
			
			virtual const char* getName() = 0;
			virtual EffectArgument* getAgrs() = 0;
			virtual const int getAgrsCount() = 0;
			
			virtual void draw(){}
			virtual void receiveClick(int X, int Y, bool begin){}
			virtual void receiveSecondClick(int X, int Y, bool begin){}
			
			void setArgument(int argId, int value);
			void setArgument(int argId, float value);
			void setArgument(int argId, std::string value);
			
			void sendArgument(int argId);
			
			void setAndSendArgument(int argId, int value);
			void setAndSendArgument(int argId, float value);
			void setAndSendArgument(int argId, std::string value);
	};
	
	bool checkEffectsList();
	
	std::map <int, Effect*>* getEffectInstanceList();
	
	void registerEffect(const char* name);

#endif