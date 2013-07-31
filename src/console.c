/*This code is part of Epoch. Epoch is maintained by Subsentient.
* This software is public domain.
* Please read the file LICENSE.TXT for more information.*/

/**This file is for console related stuff, like status reports and whatnot.
 * I can't see this file getting too big.**/

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include "epoch.h"

/*The banner we show upon startup.*/
struct _BootBanner BootBanner = { false, { '\0' }, { '\0' } };

void PrintBootBanner(void)
{ /*Real simple stuff.*/
	if (!BootBanner.ShowBanner)
	{
		return;
	}
	
	if (!strncmp(BootBanner.BannerText, "FILE", strlen("FILE")))
	{ /*Now we read the file and copy it into the new array.*/
		char *Worker, *TW, TChar;
		FILE *TempDescriptor;
		unsigned long Inc = 0;
		
		BootBanner.BannerText[Inc] = '\0';	
		
		Worker = BootBanner.BannerText + strlen("FILE");
		
		for (Inc = 0; Worker[Inc] == ' ' || Worker[Inc] == '\t'; ++Inc);
		
		Worker += Inc;
		
		if ((TW = strstr(Worker, "\n")))
		{
			*TW = '\0';
		}
		
		if (!(TempDescriptor = fopen(Worker, "r")))
		{
			char TmpBuf[1024];
			
			snprintf(TmpBuf, 1024, "Failed to display boot banner, can't open file \"%s\".", Worker);
			SpitWarning(TmpBuf);
			return;
		}
		
		for (Inc = 0; (TChar = getc(TempDescriptor)) != EOF && Inc < 512; ++Inc)
		{ /*It's a loop copy. Get over it.*/
			BootBanner.BannerText[Inc] = TChar;
		}
		BootBanner.BannerText[Inc] = '\0';
		
		fclose(TempDescriptor);
	}
	
	if (*BootBanner.BannerColor)
	{
		printf("\n%s%s%s\n", BootBanner.BannerColor, BootBanner.BannerText, CONSOLE_ENDCOLOR);
	}
	else
	{
		printf("\n%s\n", BootBanner.BannerText);
	}
}

void SetBannerColor(const char *InChoice)
{
	if (!strcmp(InChoice, "BLACK"))
	{
		strncpy(BootBanner.BannerColor, CONSOLE_COLOR_BLACK, 64);
	}
	else if (!strcmp(InChoice, "BLUE"))
	{
		strncpy(BootBanner.BannerColor, CONSOLE_COLOR_BLUE, 64);
	}
	else if (!strcmp(InChoice, "RED"))
	{
		strncpy(BootBanner.BannerColor, CONSOLE_COLOR_RED, 64);
	}
	else if (!strcmp(InChoice, "GREEN"))
	{
		strncpy(BootBanner.BannerColor, CONSOLE_COLOR_GREEN, 64);
	}
	else if (!strcmp(InChoice, "YELLOW"))
	{
		strncpy(BootBanner.BannerColor, CONSOLE_COLOR_YELLOW, 64);
	}
	else if (!strcmp(InChoice, "MAGENTA"))
	{
		strncpy(BootBanner.BannerColor, CONSOLE_COLOR_MAGENTA, 64);
	}
	else if (!strcmp(InChoice, "CYAN"))
	{
		strncpy(BootBanner.BannerColor, CONSOLE_COLOR_CYAN, 64);
	}
	else if (!strcmp(InChoice, "WHITE"))
	{
		strncpy(BootBanner.BannerColor, CONSOLE_COLOR_WHITE, 64);
	}
	else
	{ /*Bad value? Warn and then set no color.*/
		char TmpBuf[1024];
		
		BootBanner.BannerColor[0] = '\0';
		snprintf(TmpBuf, 1024, "Bad color value \"%s\" specified for boot banner. Setting no color.", InChoice);
		SpitWarning(TmpBuf);
	}
}

/*Give this function the string you just printed, and it'll print a status report at the end of it, aligned to right.*/
void PrintStatusReport(const char *InStream, rStatus State)
{
	unsigned long StreamLength, Inc = 0;
	char OutMsg[2048] = { '\0' }, IP2[256];
	char StatusFormat[1024];
	struct winsize WSize;
	
	/*Get terminal width so we can adjust the status report.*/
    ioctl(0, TIOCGWINSZ, &WSize);
    StreamLength = WSize.ws_col;
    
	strncpy(IP2, InStream, 256);
	
	switch (State)
	{
		case FAILURE:
		{
			snprintf(StatusFormat, 1024, "[%s]\n", CONSOLE_COLOR_RED "FAILED" CONSOLE_ENDCOLOR);
			break;
		}
		case SUCCESS:
		{
			snprintf(StatusFormat, 1024, "[%s]\n", CONSOLE_COLOR_GREEN "Done" CONSOLE_ENDCOLOR);
			break;
		}
		case WARNING:
		{
			snprintf(StatusFormat, 1024, "[%s]\n", CONSOLE_COLOR_YELLOW "WARNING" CONSOLE_ENDCOLOR);
			break;
		}
		default:
		{
			SpitWarning("Bad parameter passed to PrintStatusReport() in console.c.");
			return;
		}
	}
	
	switch (State)
	{ /*Take our status reporting into account, but not with the color characters and newlines and stuff, 
		because that gives misleading results due to the extra characters that you can't see.*/
		case SUCCESS:
			StreamLength -= strlen("[Done]");
			break;
		case FAILURE:
			StreamLength -= strlen("[FAILED]");
			break;
		case WARNING:
			StreamLength -= strlen("[WARNING]");
			break;
		default:
			SpitWarning("Bad parameter passed to PrintStatusReport() in console.c");
			return;
	}
	
	if (strlen(IP2) >= StreamLength)
	{ /*Keep it aligned if we are printing a multi-line report.*/
		strcat(OutMsg, "\n");
	}
	else
	{
		StreamLength -= strlen(IP2);
	}
	
	/*Appropriate spacing.*/
	for (; Inc < StreamLength; ++Inc)
	{
		strcat(OutMsg, " ");
	}
	
	strcat(OutMsg, StatusFormat);
	
	printf("%s", OutMsg);
	
	return;
}

/*Two little error handling functions. Yay!*/
void SpitError(char *INErr)
{
	fprintf(stderr, CONSOLE_COLOR_RED "Epoch: ERROR: %s\n" CONSOLE_ENDCOLOR, INErr);
}

void SpitWarning(char *INWarning)
{
	fprintf(stderr, CONSOLE_COLOR_YELLOW "Epoch: WARNING: %s\n" CONSOLE_ENDCOLOR, INWarning);
}
