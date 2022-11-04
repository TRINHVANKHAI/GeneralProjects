/*
 * common.h
 *
 *  Created on: Apr 18, 2018
 *      Author: hinoeng
 */

#ifndef COMMON_H_
#define COMMON_H_


/********************************************************/
/*  Define                                              */
/********************************************************/
//#define _DEBUG_

#define DATA_BUF_SIZE (2000)
#define PRIO_MAIN_VAL (0)

#ifndef _DEBUG_
#define DBG_PRINT( f, a... )
#else 	// _DEBUG_
#define DBG_PRINT( f, a... ) printf( f, ##a )
#endif	// _DEBUG_




#endif /* COMMON_H_ */
