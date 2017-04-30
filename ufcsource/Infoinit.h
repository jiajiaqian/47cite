/*! 
* \FILE Acc_Calc.h
* \brief ´«ÊäSHMFILE  
*  
* ÏêÏ¸ËµÃ÷
*  
* \author Hanbing
* \version 1.0
* \date 7/25/2013
*/

#ifndef _INFOINIT_
#define _INFOINIT_


#include <time.h>
#include "../BaseCoolStruct.h"
#include "..//SocketLine1.h"
	
class Infoinit{

	Infoinit();
	~Infoinit();

	
public:
	static void Calc_ClearPlateBuffer(void);
	//static void InsertPDIToPreCalcStruct(TRACKDATARAL *trackPreRal,SocketLine1Define *sock1);
	static void InsertPDIToPreCalcStruct(TRACKDATARAL *trackPreRal, PGfcEXTL2PdiData sock1,int PltIcFlgTemp);

	static void InsertModifyToPreCalcStruct(TRACKDATARAL *trackPreRal);
	static void InsertModifyToICPreCalcStruct(TRACKDATARAL *trackPreRal);
	static int GetLayoutData(TRACKDATARAL *trackPreRal);

	static int CalculatePreCalcModel(TRACKDATARAL *trackPreRal);
	static int CalculatePreCalcModelElite(TRACKDATARAL *trackPreRal);
	static int CalculatePostModel(TRACKDATARAL *trackPreRal);

};

#endif