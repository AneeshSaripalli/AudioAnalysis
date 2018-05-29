#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource.h"
#include "fft.h"
#include <mmsystem.h>

//----CODE TEST REGION---------

#define format          WAVE_FORMAT_PCM
#define channels        1
#define samples         22050
#define bitspersample   8
#define avgbitspersec   22050
#define blockalign      1
#define header          0
#define second			1000
#define tick_timer		50

const int bufferSize = samples * tick_timer / second;

char* WaveData;
WAVEHDR waveHeader;
cmplx* cmplx_ptr;

int size = 0;

void audiotest (HWND* hwnd) {
	HWAVEIN wave_handle;

	// Initializing the wave format that the audio input will receive.
	WAVEFORMATEX wave_format;
	wave_format.wFormatTag = format;
	wave_format.nChannels = channels;
	wave_format.nSamplesPerSec = samples;
	wave_format.wBitsPerSample = bitspersample;
	wave_format.nAvgBytesPerSec = avgbitspersec;
	wave_format.nBlockAlign = blockalign;
	wave_format.cbSize = header;

	int Res = waveInOpen (
		&wave_handle, WAVE_MAPPER, &wave_format,
		0, 0, CALLBACK_WINDOW);

	if (Res != MMSYSERR_NOERROR) {
		MessageBox (NULL, (LPCSTR)L"Audio Query was bad.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	int i = 0;
	for (i = 0; i < bufferSize; i++) {
		WaveData[i] = 0; // initializing all data to 0
	}

	waveHeader.dwBufferLength = bufferSize; // set the bufferlength to bufferSie
	waveHeader.dwFlags = 0;					// dwFlags need to be set to 0 before openInStart() can be called to accept input
	waveHeader.lpData = WaveData;			// setting the storage buffer to be our WaveData buffer

	Res = waveInPrepareHeader (wave_handle, &waveHeader, sizeof (WAVEHDR));

	if (Res != MMSYSERR_NOERROR)
	{
		MessageBox (*hwnd, L"Header failed..", L"Success", MB_OK | MB_ICONEXCLAMATION);
	}

	Res = waveInAddBuffer (wave_handle, &waveHeader, sizeof (WAVEHDR));

	if (Res != MMSYSERR_NOERROR)
	{
		MessageBox (*hwnd, L"Buffer Failed.", L"Success", MB_OK | MB_ICONEXCLAMATION);
	}

	Res = waveInStart (wave_handle);

	if (Res != MMSYSERR_NOERROR)
	{
		MessageBox (*hwnd, L"Recording failed.", L"Success", MB_OK | MB_ICONEXCLAMATION);
	}

	FILE *f;

	int a = 1;
	while (a < 100)
	{
		int i = 0;
		fopen_s (&f, "client.txt", "wb");
		for (i = 0; i < bufferSize; i++)
		{
			fprintf (f, "%d ", WaveData[i] + 128);
		}

		fflush (f);
		fclose (f);

		printf ("\n");
		a++;
	}

	//MessageBox (*hwnd, L"Beginning transform.", L"Success", MB_OK | MB_ICONEXCLAMATION);


	for (i = 0; i < bufferSize; i++)
	{
		cmplx_ptr[i] = __cmplx ((int)WaveData[i], 0.);
	}

	for (i = bufferSize; i < size; i++)
	{
		cmplx_ptr[i] = __cmplx (0., 0.);
	}

	fft (cmplx_ptr, size);

	fopen_s (&f, "results.txt", "wb");

	double max = -1000000;
	for (i = 0; i < size; i++)
	{
		max = max > cmplx_ptr[i].imag / (double)size ? max : cmplx_ptr[i].imag / (double)size;
	}

	fprintf (f, "max found %f\n", max);

	for (i = 0; i < size; i++) {
		fprintf (f, "%3d\t%3d\t%+.3f\t%d\n", i, ((int)WaveData[i] + 128), (cmplx_ptr[i]).imag / (double)(size * max), i);
	}


	//MessageBox (*hwnd, L"Finished recording.", L"Success", MB_OK | MB_ICONEXCLAMATION);

	fclose (f);

	//MessageBox (*hwnd, L"Closed everything returning.", L"Success", MB_OK | MB_ICONEXCLAMATION);
}

///////////////////////

typedef struct tagRect {
	double x, y, width, height;
} Rect;

const char g_szClassName[] = "myWindowClass";
const int WINDOW_WIDTH = 1366;
const int WINDOW_HEIGHT = 768;
const int TIMER_ID = 1;

int i = 0;
int j = 0;

HBITMAP g_ball = NULL;

Rect *freqs = 0;

/**
 * Function protoypes:
 */
void drawPoint (HDC *, LONG, LONG, LONG, LONG);

void initRect (Rect *, double x, double y, double width, double height);

void renderRect (HDC *, Rect *);

/**
 * Render function called by WndProc when WM_Paint message is passed. Handles all signal rendering
 * @param hwnd  Pointer to the Window Handle associated with the frame
 * @param ps    Pointer to a PAINTSTRUCT object that is associated with the screen & canvas
 * @param hdc   Pointer to handle to a Device Context associated with the Window Display
 */
void render (HWND *hwnd, PAINTSTRUCT *ps, HDC *hdc) {
	HBRUSH NewBrush;

	NewBrush = CreateSolidBrush (RGB ((i) % 255, (2 * i) % 255, (3 * i) % 255));

	SelectObject (*hdc, NewBrush);


	double max = -1000000;
	for (i = 0; i < size; i++)
	{
		max = max > cmplx_ptr[i].imag / (double)size ? max : cmplx_ptr[i].imag / (double)size;
	}

	double freqs_per_pixel = 2 * WINDOW_WIDTH / size;


	Rect rect;
	for (i = 0; i < size / 2; i++) {

		double draw_value = (cmplx_ptr[i].imag) / (double)(size * max); // scaled to a maximum of 1.0000
		double px_height = WINDOW_HEIGHT * draw_value;

		initRect (&rect, (int)(i * freqs_per_pixel), WINDOW_HEIGHT - (int)(px_height), 1, (int)px_height);

		renderRect (hdc, &rect);
		//		fprintf (f, "%3d\t%3d\t%+.3f\t%d\n", i, ((int)WaveData[i] + 128), (cmplx_ptr[i]).imag / (double)(size * max), i);
	}

	DeleteObject (NewBrush);

	EndPaint (*hwnd, ps);
}

void initRect (Rect *rect, double x, double y, double width, double height) {
	rect->x = x;
	rect->y = y;
	rect->width = width;
	rect->height = height;
}

void renderRect (HDC *hdc, Rect *rectPtr) {
	int min_x = (int)rectPtr->x;
	int min_y = (int)rectPtr->y;
	int max_x = min_x + (int)rectPtr->width;
	int max_y = min_y + (int)rectPtr->height;

	Rectangle (*hdc, min_x, min_y, max_x, max_y);
}


// Step 4: the Window Procedure
LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_CREATE: {
		g_ball = LoadBitmap (GetModuleHandle (NULL), MAKEINTRESOURCE (IDB_BMP));
		if (g_ball == NULL)
			MessageBox (hwnd, "Could not load bmp.bmp!", "ERROR", MB_OK | MB_ICONEXCLAMATION);

		SetTimer (hwnd, 0, tick_timer, NULL);

		size = 1;

		while (size < bufferSize)
			size *= 2;

		WaveData = (char*)malloc (bufferSize * sizeof (char)); // allocating memory region to accept audio signal input
		cmplx_ptr = (cmplx*)malloc (sizeof (cmplx) * size);

		break;
	}
	case WM_TIMER: {
		audiotest (&hwnd);

		InvalidateRect (hwnd, NULL, TRUE);

		break;
	}
	case WM_PAINT: {

		printf ("Paint called\n");

		HDC hDC;
		PAINTSTRUCT Ps;

		hDC = BeginPaint (hwnd, &Ps);

		render (&hwnd, &Ps, &hDC);


		break;
	}
	case WM_CLOSE:

		DestroyWindow (hwnd);
		break;
	case WM_DESTROY:
		free (cmplx_ptr);
		free (WaveData);
		KillTimer (hwnd, TIMER_ID);
		PostQuitMessage (0);
		break;
	default:
		return DefWindowProc (hwnd, msg, wParam, lParam);
	}
	return 0;
}

void drawPoint (HDC *hdc, LONG sx,
				LONG sy, LONG
				fx,
				LONG fy
) {
	MoveToEx (*hdc, sx, sy, (LPPOINT)NULL);
	LineTo (*hdc, fx, fy);
}


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, int nCmdShow) {

	SetThreadLocale (LOCALE_USER_DEFAULT);

	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	//Step 1: Registering the Window Class
	wc.cbSize = sizeof (WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon (GetModuleHandle (NULL), MAKEINTRESOURCE (IDI_MYICON));
	wc.hIconSm = (HICON)LoadImage (GetModuleHandle (NULL), MAKEINTRESOURCE (IDI_MYICON), IMAGE_ICON, 32, 32, 0);
	wc.hCursor = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = MAKEINTRESOURCE (IDR_MYMENU);
	wc.lpszClassName = g_szClassName;

	if (!RegisterClassEx (&wc)) {
		MessageBox (NULL, L"Window Registration Failed!", L"Error!",
					MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// creates the window for the screen display
	hwnd = CreateWindowEx (
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"The title of my window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) // checks if the window handle in null, in which case it exits
	{
		MessageBox (NULL, L"Window Creation Failed!", L"Error!",
					MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow (hwnd, nCmdShow);
	UpdateWindow (hwnd);

	// Step 3: The Message Loop
	while (GetMessage (&Msg, NULL, 0, 0) > 0) {
		TranslateMessage (&Msg);
		DispatchMessage (&Msg);
	}
	return Msg.wParam;
}
