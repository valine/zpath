#ifndef ZWORLD_H
#define ZWORLD_H

#include "ui/ztexture.h"
#include <string>

using namespace std;

class ZWorld {
	public:

		void setEnvironmentTexture(ZTexture* texture);

	private:
		ZTexture* mEnvironmentTexture;



};

#endif