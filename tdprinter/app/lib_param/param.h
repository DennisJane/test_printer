#ifndef __FILE_APP_PARAM_PARAM_H__
#define __FILE_APP_PARAM_PARAM_H__

#include <stdint.h>


typedef struct
{
    // 参数信息，pos=0。
    uint32_t version; // 参数结构的版本。
    
    uint32_t crc;
} param_t;

int32_t         param_init(void);
const param_t * param_getRamParam(void);
param_t *       param_setRamParam(void);
const param_t * param_getFlashParam(void);
int32_t         param_writeToFlash(void);
int32_t         param_resetRam(void);
int32_t			param_loadFlash(void);

#endif // __FILE_APP_PARAM_PARAM_H__
