// AudioSeparatorFile.h: interface for the CAudioSeparatorFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOSEPARATORFILE_H__2110E737_882D_48D3_9107_E53797331BD7__INCLUDED_)
#define AFX_AUDIOSEPARATORFILE_H__2110E737_882D_48D3_9107_E53797331BD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "ProgressWnd.h"
#include "riff.h"

#define INITIAL_VECTOR_SIZE 100
#define MAXIMUM_VECTOR_SIZE 100000
#define MAX_SPLITS_FOR_UNREGISTERED_USERS 5

extern BOOLEAN g_bRegisteredUser;

enum AUDSEPRET
{
   AUD_SUCCESS,                 // The operation succeeded
   AUD_FAILURE,                 // The operation failed for unspecified reasons
   AUD_FILE_OPEN_FAILED,        // Couldn't open the specified file
   AUD_TOO_SHORT_FOR_SPLIT,     // The audio file is too short to split
   AUD_PROCESS_CANCELLED,       // user cancelled the analysis
   AUD_NO_SPLIT_POINTS_FOUND    // user should adjust parameters
};

enum SILENCE_DETECTION_METHOD
{
	METHOD_AVERAGE_NOISE,
	METHOD_MAXIMUM_NOISE
};

enum POINT_TO_LOOK_FOR
{
	SOUND,
	SILENCE
};

class CAudioSeparatorFile  
{
protected:
	CString m_strBeginWith;
	CProgressWnd m_wndProgress;
	unsigned long m_nAppendSilence;
	void MakeSplit(INT64 nNextSplitPoint);
	INT64 FindNextPoint(POINT_TO_LOOK_FOR p);
	CWnd *m_parentWnd;
	WaveFile m_wavFile;
	INT64 m_nSamplesForMinTrackLength, m_nCurrentSample, m_nBeginTrim, m_nEndTrim,
	      m_nMaxSplitLength, m_nMinSplitLength, m_nAvgSplitLength;
	unsigned long m_nChunkSize, m_nSplitCount;
	double m_nSilenceThreshold;
	std::vector<UINT64> m_SplitPoints;
	SILENCE_DETECTION_METHOD m_method;
	BOOL m_bTrimFile;

public:
	void SplitFile();
	AUDSEPRET AnalyzeAudio();
	CAudioSeparatorFile();
	AUDSEPRET Open(const char *Filename,                // filename to open
	               unsigned long nMinSilenceLength,     // in milliseconds
	               unsigned long nMinTrackLength,            // in seconds
				   double nSilenceThreshold,            // fractional real number (0.45, for example)
				   unsigned long nAppendSilence,        // number of milliseconds of silence to append
                   SILENCE_DETECTION_METHOD method,
                   BOOL bTrimFile,
	               const char *szBeginWith,
                   CWnd *parentWnd                      // for displaying progress
				   );
	virtual ~CAudioSeparatorFile();

};

#endif // !defined(AFX_AUDIOSEPARATORFILE_H__2110E737_882D_48D3_9107_E53797331BD7__INCLUDED_)
