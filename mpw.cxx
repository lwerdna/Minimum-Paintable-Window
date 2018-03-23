#include <stdio.h> // stdin, printf, getline()
#include <unistd.h> // STDIN_FILENO
#include <stdint.h> // uint8_t, etc.
#include <stdlib.h> // malloc()/free()

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>

#include "base64.h"

#define POS_X 100
#define POS_Y 100
#define SIZE_X 200
#define SIZE_Y 200

class MyWin : public Fl_Double_Window
{
	private:
	uint8_t *canvasData = NULL;
	Fl_RGB_Image *canvasImage = NULL;

    public:
    MyWin(int x, int y, int w_, int h_):
        Fl_Double_Window(x, y, w_, h_)
    {
        printf("MyWin constructor\n");

		canvasData = (uint8_t *)malloc(w_*h_*3);
		memset(canvasData, 0, w_*h_*3);
        canvasImage = new Fl_RGB_Image(canvasData, w_, h_, 3);
    }

	void paint(int locX, int locY, int srcW, int srcH, uint8_t *data)
	{
		int dstW = w();
		int dstH = h();

		/* full screen replacements optimize to a memcpy */
		if(locX==locY==0 && srcW==dstW && srcH==dstH) {
			memcpy(canvasData, data, dstW * dstH * 3);
		}
		else {
			/* horizontal clipping */
			int spanX = (locX + srcW < dstW) ? srcW : (dstW - locX);

			int srcX=0, srcY=0;
			int dstX=locX, dstY=locY;
			while(1) {
				/* south of destination? (vertical clipping) */
				if(dstY >= dstH)
					break;

				/* south of source? */
				if(srcY >= srcH)
					break;

				int srcIdx = srcX + srcW * srcY;
				int dstIdx = dstX + dstW * dstY;

				printf("copying (%d,%d) -> (%d,%d) (%d bytes)\n",
					srcX, srcY, dstX, dstY, spanX);
				memcpy(canvasData + 3*dstIdx, data + 3*srcIdx, 3*spanX);

				srcY++;
				dstY++;
			}
		}

		/* new image from the modified data */
		delete canvasImage;
        canvasImage = new Fl_RGB_Image(canvasData, dstW, dstH, 3);
	}

    int handle(int event)
    {
        int rc = 0; /* 0 if not used or understood, 1 if event was used and can be deleted */

        switch(event) {
        	case FL_NO_EVENT:
        		break;
        	case FL_PUSH:
        		printf("PUSH %d at %d,%d\n", Fl::event_button(), Fl::event_x(), Fl::event_y());
        		rc = 1;
        		break;
        	case FL_RELEASE:
        		printf("RELEASE %d at %d,%d\n", Fl::event_button(), Fl::event_x(), Fl::event_y());
        		rc = 1;
        		break;
            case FL_ENTER:
        		printf("ENTER at %d,%d\n", Fl::event_x(), Fl::event_y());
        		rc = 1;
        		break;
            case FL_LEAVE:
        		printf("LEAVE %d,%d\n", Fl::event_x(), Fl::event_y());
        		rc = 1;
        		break;
            case FL_KEYDOWN:
        		printf("KEYDOWN %d\n", Fl::event_key());
        		rc = 1;
        		break;
            case FL_KEYUP:
        		printf("KEYUP %d\n", Fl::event_key());
        		rc = 1;
        		break;
            case FL_PASTE:
                printf("got paste event");
                printf("event text: %s\n", Fl::event_text());
                printf("event length: %d\n", Fl::event_length());
                rc = 1;
                break;

            default:
                while(0);
                //printf("got event id: %d\n", event);
        }

        return rc;
    }

    void draw(void)
    {
		if(!canvasImage)
			return;

		canvasImage->draw(0, 0);

		return;
    }
};

void cb_fileDescrReady(FL_SOCKET fd, void *cbparam)
{
	uint8_t *data = NULL;
	char *line = NULL;
	size_t nalloc = 0;
	unsigned long x, y, w, h;
	int len;
	MyWin *win = (MyWin *)cbparam;

	if(getline(&line, &nalloc, stdin) == -1) {
		printf("ERROR getline()\n");
		goto cleanup;
	}

	len = strlen(line); // len was allocated length, not string length
	while(line[len-1]==0x0d || line[len-1]==0x0a) {
		line[len-1] = '\0';
		len -= 1;
	}

	/* INSERT command
		 syntax: INSERT x,y width,height base64data
		example: INSERT 123,456 30,30 base64datablahblah== */
	if(strncmp(line, "INSERT ", 7) == 0) {
		char *px, *py, *pw, *ph, *pdata, *endptr;
		unsigned long x, y, w, h;
		uint8_t *data;

		px = line+7;
		x = strtoul(px, &endptr, 10);
		if(x==0 && px==endptr)
			goto cleanup;

		if(*endptr != ',')
			goto cleanup;
		py = endptr+1;

		y = strtoul(py, &endptr, 10);
		if(y==0 && (py == endptr))
			goto cleanup;

		if(*endptr != ' ')
			goto cleanup;
		pw = endptr+1;

		w = strtoul(pw, &endptr, 10);
		if(w==0 && pw==endptr)
			goto cleanup;

		if(*endptr != ',')
			goto cleanup;
		ph = endptr+1;

		h = strtoul(ph, &endptr, 10);
		if(h==0 && ph==endptr)
			goto cleanup;

		if(*endptr != ' ')
			goto cleanup;
		pdata = endptr+1;

		data = base64_decode(pdata, strlen(pdata));
		if(!data) {
			printf("ERROR base64_decode()\n");
			goto cleanup;
		}

		printf("inserting at %lu,%lu size %lu,%lu data: %s\n", x,y,w,h,pdata);
		win->paint(x, y, w, h, data);
	}
	else
	/* SHOW command
		 syntax: "SHOW"
	*/
	if(strncmp(line, "SHOW", 4) == 0) {
		printf("showing double buffer\n");
		win->redraw();
	}
	else
	{
		printf("ERROR: parsing incoming line: %s", line);
	}

	cleanup:
	if(line)
		free(line);
	if(data)
		free(data);
}

int main(int ac, char **av) {
	unsigned long width = SIZE_X;
	unsigned long height = SIZE_Y;

	for(int i=0; i<ac; ++i) {
		if(0==strcmp(av[i], "--width") && (i+1)<ac)
			width = strtoul(av[i+1], NULL, 10);
		if(0==strcmp(av[i], "--height") && (i+1)<ac)
			height = strtoul(av[i+1], NULL, 10);
	}

	/* create the window */
	printf("creating window sized %lux%lu\n", width, height);
	Fl_Double_Window *win = new MyWin(POS_X, POS_Y, width, height);
	win->end();
	win->show(0, NULL);

	/* set up stdin handler */
	Fl::add_fd(STDIN_FILENO, FL_READ, cb_fileDescrReady, win);

	/* run */
	Fl::run();
	delete win;
	return 0;
}

