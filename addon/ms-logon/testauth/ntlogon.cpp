/////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2002-2024 UltraVNC Team Members. All Rights Reserved.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
//  If the source code for the program is not available from the place from
//  which you received this file, check
//  https://uvnc.com/
//
////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

typedef BOOL (*CheckUserGroupPasswordFn)( char * userin,char *password,char *machine,char *group,int lodom);
CheckUserGroupPasswordFn CheckUserGroupPassword = 0;
bool CheckAD()
{
	HMODULE hModule = LoadLibrary("Activeds.dll");
	if (hModule)
	{
		FreeLibrary(hModule);
		return true;
	}
	return false;
}

bool CheckNetapi95()
{
	HMODULE hModule = LoadLibrary("netapi32.dll");
	if (hModule)
	{
		FreeLibrary(hModule);
		return true;
	}
	return false;
}

bool CheckDsGetDcNameW()
{
	HMODULE hModule = LoadLibrary("netapi32.dll");
	if (hModule)
	{
		FARPROC test=NULL;
		test=GetProcAddress( hModule, "DsGetDcNameW" );
		FreeLibrary(hModule);
		if (test) return true;
	}
	return false;
}

bool CheckNetApiNT()
{
	HMODULE hModule = LoadLibrary("radmin32.dll");
	if (hModule)
	{
		FreeLibrary(hModule);
		return true;
	}
	return false;
}

int main(int argc, char* argv[])
{
	char pszBuffer[256];
	char pszBuffer2[256];
	char pszBuffer3[256];
	char pszBuffer4[256];
	int locdom;
	if (argc<5) 
	{
		printf("Enter user name : ");
		fgets(pszBuffer,256,stdin);
		printf("\nEnter password : ");
		fgets(pszBuffer2, 256, stdin);
		printf("\n");
		printf("Enter group : ");
		fgets(pszBuffer3, 256, stdin);
		printf("\n");
		printf("loc=1 dom=3 local+domain=3 : ");
		fgets(pszBuffer4, 256, stdin);
		printf("\n");
	if (0==strcmp("", pszBuffer)) 
	{
		return 0;
	}
	if (0==strcmp("", pszBuffer2)) 
	{
		return 0;
	}
	if (0==strcmp("", pszBuffer3)) 
	{
		return 0;
	}
	if (0==strcmp("", pszBuffer4)) 
	{
		return 0;
	}
	}
	else
	{
		strcpy_s(pszBuffer,argv[1]);
		strcpy_s(pszBuffer2,argv[2]);
		strcpy_s(pszBuffer3,argv[3]);
		strcpy_s(pszBuffer4,argv[4]);
	}
	locdom=atoi(pszBuffer4);
	if (!CheckNetapi95() && !CheckNetApiNT())
	{
		printf("Netapi not found,radmin32.dll is missing \n");
	}

	///////////////////////////////////////
	////// BASIC LOCAL GROUPS
	BOOL returnvalue;
	{
	returnvalue=false;
	printf("\nTrying authlogonuser.dll \n");
		printf("This only works on Windows XP> \n");
		printf("test is runnning as application and not a service \n");
		printf("------------------------ \n");
		printf("------------------------ \n");
		HMODULE hModule = LoadLibrary("authlogonuser.dll");
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				returnvalue=CheckUserGroupPassword(pszBuffer,pszBuffer2,"test",pszBuffer3,locdom);
				CoUninitialize();
				FreeLibrary(hModule);
			}
		else printf("authlogonuser.dll not found");
	if (returnvalue==1) printf("based on auth.dll user has access ");
	else printf("based on authlogonuser.dll user has NO access ");
	}
	if (returnvalue==1) printf("\n-------------------------\n");
	if (returnvalue==1) printf("For testing we cont, but access was already granted\n");
	if (returnvalue==1) printf("-------------------------\n");

	returnvalue=false;
	printf("\nTrying auth.dll \n");
		printf("------------------------ \n");
		printf("------------------------ \n");
		HMODULE hModule = LoadLibrary("auth.dll");
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				returnvalue=CheckUserGroupPassword(pszBuffer,pszBuffer2,"test",pszBuffer3,locdom);
				CoUninitialize();
				FreeLibrary(hModule);
			}
		else printf("auth.dll not found");
	if (returnvalue==1) printf("based on auth.dll user has access ");
	else printf("based on auth.dll user has NO access ");
	if (returnvalue==1) printf("\n-------------------------\n");
	if (returnvalue==1) printf("For testing we cont, but access was already granted\n");
	if (returnvalue==1) printf("-------------------------\n");
	returnvalue=false;
	if (CheckAD())
	{
		printf("\nTrying authad.dll \n");
		printf("------------------------ \n");
		printf("------------------------ \n");
		HMODULE hModule = LoadLibrary("authad.dll");
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				returnvalue=CheckUserGroupPassword(pszBuffer,pszBuffer2,"test",pszBuffer3,locdom);
				CoUninitialize();
				FreeLibrary(hModule);
			}
		else printf("authad.dll not found");
		if (returnvalue==1) printf("based on authad.dll user has access ");
		else printf("based on authad.dll user has NO access ");

	}
	if (returnvalue==1) printf("\n-------------------------\n");
	if (returnvalue==1) printf("For testing we cont, but access was already granted\n");
	if (returnvalue==1) printf("-------------------------\n");
	if (locdom==2 || locdom==3)
	{
	returnvalue=false;
	OSVERSIONINFO VerInfo;
	VerInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	if (!GetVersionEx (&VerInfo))   // If this fails, something has gone wrong
	{
		  return FALSE;
	}
	if (VerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && VerInfo.dwMajorVersion == 4)
	{
		if (CheckAD() && CheckDsGetDcNameW())
		{
			printf("\nTrying ldapauthNT4.dll \n");
		printf("------------------------ \n");
		printf("------------------------ \n");
		HMODULE hModule = LoadLibrary("ldapauthnt4.dll");
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				returnvalue=CheckUserGroupPassword(pszBuffer,pszBuffer2,"test",pszBuffer3,locdom);
				CoUninitialize();
				FreeLibrary(hModule);
			}
		else printf("ldapauthnt4.dll not found");
		if (returnvalue==1) printf("based on ldapauthnt4.dll user has access ");
		else printf("based on ldapauthnt4.dll user has NO access ");
		}

	}
	if (returnvalue==1) printf("\n-------------------------\n");
	if (returnvalue==1) printf("For testing we cont, but access was already granted\n");
	if (returnvalue==1) printf("-------------------------\n");
	returnvalue=false;
	if (VerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && VerInfo.dwMajorVersion >= 5)
	{
		if (CheckAD())
		{
			printf("\nTrying ldapauth.dll \n");
		printf("------------------------ \n");
		printf("------------------------ \n");
		HMODULE hModule = LoadLibrary("ldapauth.dll");
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				returnvalue=CheckUserGroupPassword(pszBuffer,pszBuffer2,"test",pszBuffer3,locdom);
				CoUninitialize();
				FreeLibrary(hModule);
			}
		else printf("ldapauth.dll not found");
		if (returnvalue==1) printf("based on ldapauth.dll user has access ");
		else printf("based on ldapauth.dll user has NO access ");
		}
	}
	}
	printf("Enter to quit ");
	fgets(pszBuffer, 256, stdin);
	return 0;
}
