#include "stdafx.h"
//-----------------------------------------------------------------------------
// File: RageUtil.cpp
//
// Desc: 
//
// Copyright (c) 2001 Chris Danford.  All rights reserved.
//-----------------------------------------------------------------------------

#include "RageUtil.h"


//-----------------------------------------------------------------------------
// UTIL globals
//-----------------------------------------------------------------------------
const CString g_sLogFileName = "debug.log";


//-----------------------------------------------------------------------------
// Name: randomf()
// Desc:
//-----------------------------------------------------------------------------
FLOAT randomf( FLOAT low, FLOAT high )
{
    return low + ( high - low ) * ( (FLOAT)rand() ) / RAND_MAX;
}

//-----------------------------------------------------------------------------
// Name: roundf()
// Desc:
//-----------------------------------------------------------------------------
int roundf( FLOAT f )
{
    return (int)((f)+0.5f);
}

int roundf( double f )
{
    return (int)((f)+0.5f);
}

//-----------------------------------------------------------------------------
// Name: RageLogStart()
// Desc:
//-----------------------------------------------------------------------------
void RageLogStart()
{
	// delete the old log and create a new one
	DeleteFile( g_sLogFileName );
	FILE *fp = NULL;
    fp = fopen( g_sLogFileName, "w" );
	fclose( fp );


	SYSTEMTIME st;
    GetLocalTime( &st );

	RageLog( "%s: last compiled on %s.", g_sLogFileName, __TIMESTAMP__ );
	RageLog( "Log starting %.4d-%.2d-%.2d %.2d:%.2d:%.2d", 
		     st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
	RageLog( "\n" );
}


//-----------------------------------------------------------------------------
// Name: RageLog()
// Desc:
//-----------------------------------------------------------------------------
void RageLog( LPCTSTR fmt, ...)
{

#if defined(DEBUG) | defined(_DEBUG)
    va_list	va;
    va_start(va, fmt);

    CString sBuff = vssprintf( fmt, va );
	sBuff += "\n";

	FILE *fp = NULL;

	// open our logging file
    fp = fopen( g_sLogFileName, "a" );
    if( fp==NULL ) 
		RageLog( ssprintf("Couldn't write to log %s", g_sLogFileName) );

	fprintf(fp, sBuff);

	fclose(fp);
#endif

}


//-----------------------------------------------------------------------------
// Name: ssprintf()
// Desc:
//-----------------------------------------------------------------------------
CString ssprintf( LPCTSTR fmt, ...)
{
    va_list	va;
    va_start(va, fmt);
	return vssprintf(fmt, va);
}


//-----------------------------------------------------------------------------
// Name: vssprintf()
// Desc:
//-----------------------------------------------------------------------------
CString vssprintf( LPCTSTR fmt, va_list argList)
{
	CString str;
	str.FormatV(fmt, argList);
	return str;
}



//-----------------------------------------------------------------------------
// Name: join()
// Desc:
//-----------------------------------------------------------------------------
CString join(CString Deliminator, CStringArray& Source)
{
	CString csReturn;
	CString csTmp;

	// Loop through the Array and Append the Deliminator
	for( int iNum = 0; iNum < Source.GetSize(); iNum++ ) {
		csTmp += Source.GetAt(iNum);
		csTmp += Deliminator;
	}
	csReturn = csTmp.Left(csTmp.GetLength() - 1);
	return csReturn;
}


//-----------------------------------------------------------------------------
// Name: split()
// Desc:
//-----------------------------------------------------------------------------
void split( CString Source, CString Deliminator, CStringArray& AddIt )
{
	CString		 newCString;
	CString		 tmpCString;
	CString		 AddCString;

	int pos1 = 0;
	int pos = 0;

	newCString = Source;
	do {
		pos1 = 0;
		pos = newCString.Find(Deliminator, pos1);
		if ( pos != -1 ) {
			CString AddCString = newCString.Left(pos);
			if (!AddCString.IsEmpty())
				AddIt.Add(AddCString);

			tmpCString = newCString.Mid(pos + Deliminator.GetLength());
			newCString = tmpCString;
		}
	} while ( pos != -1 );

	if (!newCString.IsEmpty())
		AddIt.Add(newCString);
}


//-----------------------------------------------------------------------------
// Name: splitpath()
// Desc:
//-----------------------------------------------------------------------------
void splitpath (BOOL UsingDirsOnly, CString Path, CString& Drive, CString& Dir, CString& FName, CString& Ext)
{

	int nSecond;

	// Look for a UNC Name!
	if (Path.Left(2) == "\\\\") {
		int nFirst = Path.Find("\\",3);
		nSecond = Path.Find("\\",nFirst + 1);
		if (nSecond == -1) {
			Drive = Path;
			Dir = "";
			FName = "";
			Ext = "";
		}
		else if (nSecond > nFirst)
			Drive = Path.Left(nSecond);
	}
	else { // Look for normal Drive Structure
		nSecond = 2;
		Drive = Path.Left(2);
	}

	if (UsingDirsOnly) {
		Dir = Path.Right((Path.GetLength() - nSecond) - 1);
		FName = "";
		Ext = "";
	}
	else {
		int nDirEnd = Path.ReverseFind('\\');
		if (nDirEnd == Path.GetLength()) {
			Dir = "";
			FName = "";
			Ext = "";
		}
		else {

			Dir = Path.Mid(nSecond + 1, (nDirEnd - nSecond) - 1);

			int nFileEnd = Path.ReverseFind('.');
			if (nFileEnd != -1) {
				
				if (nDirEnd > nFileEnd) {
					FName = Path.Right(Path.GetLength() - nDirEnd);
					Ext = "";
				}
				else {
					FName = Path.Mid(nDirEnd + 1, (nFileEnd - nDirEnd) - 1);
					Ext = Path.Right((Path.GetLength() - nFileEnd) - 1);
				}
			}
			else {
				FName = Path.Right((Path.GetLength() - nDirEnd) - 1);
				Ext = "";
			}
		}
	}
}

void GetDirListing( CString sPath, CStringArray &AddTo, BOOL bOnlyDirs )
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile( sPath, &fd );

	if( INVALID_HANDLE_VALUE != hFind )
	{
		do
		{
			if( !bOnlyDirs  ||  fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				CString sDirName( fd.cFileName );
				if( sDirName != "."  &&  sDirName != ".." )
					AddTo.Add( sDirName );
			}
		} while( ::FindNextFile( hFind, &fd ) );
		::FindClose( hFind );
	}
}

BOOL DoesFileExist( CString sPath )
{
	RageLog( "DoesFileExist(%s)", sPath );

    DWORD dwAttr = GetFileAttributes( sPath );
    if( dwAttr == (DWORD)-1 )
        return FALSE;
	else
		return TRUE;

}

int CompareCStrings(const void *arg1, const void *arg2)
{
	CString str1 = *(CString *)arg1;
	CString str2 = *(CString *)arg2;
	return str1.CompareNoCase( str2 );
}

void SortCStringArray( CStringArray &AddTo, BOOL bSortAcsending )
{
	qsort( AddTo.GetData(), AddTo.GetSize(), sizeof(CString), CompareCStrings );
}

//-----------------------------------------------------------------------------
// Name: DisplayErrorAndDie()
// Desc:
//-----------------------------------------------------------------------------

VOID DisplayErrorAndDie( CString sError )
{
	RageLog( "" );
	RageLog( "// Fatal Error /////////////////////////////" );
	RageLog( sError );
	RageLog( "////////////////////////////////////////////" );

	// Something very bad happened.  Display an error dialog, then exit right away.
	MessageBox(
	  NULL,					// handle of owner window
	  sError,			// text in message box
	  "Fatal Error",		// address of title of message box
	  MB_OK | MB_ICONERROR	// style of message box
	);
	exit(1);
}
