#ifndef UISP_H
#define UISP_H

#define run_released() (PINC & 2)
#define usr_released() (PINC & 1)


struct uisp_app
{
	char name[16];
	char version[16];
};


#endif
