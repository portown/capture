// defines.h

#ifndef DEFINITIONS_HEADER
#define DEFINITIONS_HEADER

#include "model/picture.hpp"


// ==============================================
// ��`
// ==============================================

#define ID_MYTAB 100

struct DCSET
{
  std::shared_ptr<capture::model::picture> picture;
	BOOL bSave;
};


#endif


// EOF
